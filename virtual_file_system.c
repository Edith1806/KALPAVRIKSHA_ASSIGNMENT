#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE      512
#define MAX_BLOCKS      5000
#define DEFAULT_BLOCKS  1024
#define MAX_NAME        50
#define LINE_LEN        4096


typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME + 1];
    int  isDirectory;               
    int  size;                      
    int *blockPointers;             
    int  blockCount;                
    struct FileNode *parent;       
    struct FileNode *child;         
    struct FileNode *next;          
    struct FileNode *prev;
} FileNode;

static unsigned char virtualDisk[MAX_BLOCKS][BLOCK_SIZE];
static int numBlocks = DEFAULT_BLOCKS;
static int freeCount = 0;

static FreeBlock *freeHead = NULL;
static FreeBlock *freeTail = NULL;
static FileNode *root = NULL;
static FileNode *cwd  = NULL;

static void appendFreeBlock(int index)
{
    FreeBlock *fb = (FreeBlock *)malloc(sizeof(FreeBlock));
    if (!fb) 
    { fprintf(stderr, "Out of memory\n"); 
      exit(1); 
    }
    fb->index = index;
    fb->next = NULL;
    fb->prev = freeTail;
    if (freeTail) 
        freeTail->next = fb;
    else  
        freeHead = fb;
    freeTail = fb;
    freeCount++;
}

static int allocBlock(void)
{
    if (!freeHead) return -1;
    FreeBlock *fb = freeHead;
    int idx = fb->index;
    freeHead = fb->next;
    if (freeHead) 
        freeHead->prev = NULL;
    else
        freeTail = NULL;
    free(fb);
    freeCount--;
    return idx;
}

static void initDisk(void)
{
    memset(virtualDisk, 0, sizeof(virtualDisk));
    for (int i = 0; i < numBlocks; i++)
        appendFreeBlock(i);
}

static void releaseFileBlocks(FileNode *f)
{
    for (int i = 0; i < f->blockCount; i++)
    {
        memset(virtualDisk[f->blockPointers[i]], 0, BLOCK_SIZE);
        appendFreeBlock(f->blockPointers[i]);  
    }
    free(f->blockPointers);
    f->blockPointers = NULL;
    f->blockCount = 0;
    f->size = 0;
}

static void destroyFreeList(void)
{
    while (freeHead) {
        FreeBlock *t = freeHead;
        freeHead = freeHead->next;
        free(t);
    }
    freeTail = NULL;
    freeCount = 0;
}

static FileNode *newNode(const char *name, int isDir, FileNode *parent)
{
    FileNode *n = (FileNode *)calloc(1, sizeof(FileNode));
    if (!n) { fprintf(stderr, "Out of memory\n"); exit(1); }
    strncpy(n->name, name, MAX_NAME);
    n->name[MAX_NAME] = '\0';
    n->isDirectory = isDir;
    n->parent = parent;
    return n;
}
static void insertChild(FileNode *dir, FileNode *node)
{
    if (!dir->child) 
    {                
        node->next = node->prev = node;
        dir->child = node;
    } 
    else 
    {                           
        FileNode *head = dir->child;
        FileNode *tail = head->prev;
        node->next = head;
        node->prev = tail;
        tail->next = node;
        head->prev = node;
    }
}
static void removeChild(FileNode *dir, FileNode *node)
{
    if (node->next == node) 
    {          
        dir->child = NULL;
    }
    else 
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (dir->child == node) dir->child = node->next;
    }
    node->next = node->prev = NULL;
}

static FileNode *findChild(FileNode *dir, const char *name)
{
    if (!dir->child) return NULL;
    FileNode *cur = dir->child;
    do 
    {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    } while (cur != dir->child);
    return NULL;
}

static void freeTree(FileNode *node)
{
    if (!node) return;
    if (node->isDirectory && node->child) {
        FileNode *cur = node->child;
        node->child->prev->next = NULL;     
        while (cur) 
        {
            FileNode *nxt = cur->next;
            freeTree(cur);
            cur = nxt;
        }
        node->child = NULL;
    }
    free(node->blockPointers);
    free(node);
}

static void buildPath(FileNode *node, char *out)
{
    if (node == root) { strcpy(out, "/"); return; }
    char tmp[LINE_LEN];
    buildPath(node->parent, tmp);
    if (strcmp(tmp, "/") == 0) 
        snprintf(out, LINE_LEN, "/%s",node->name);
    else                       
        snprintf(out, LINE_LEN, "%s/%s", tmp,node->name);
}

static int validName(const char *name)
{
    size_t len = strlen(name);
    if (len == 0 || len > MAX_NAME) 
    {
        printf("Error: name must be 1-%d characters.\n", MAX_NAME);
        return 0;
    }
    if (strchr(name, '/')) 
    {
        printf("Error: name cannot contain '/'.\n");
        return 0;
    }
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
    {
        printf("Error: invalid name.\n");
        return 0;
    }
    return 1;
}
static void cmdMkdir(const char *name)
{
    if (!validName(name)) return;
    if (findChild(cwd, name)) { printf("Error: '%s' already exists.\n", name); return; }
    insertChild(cwd, newNode(name, 1, cwd));
    printf("Directory '%s' created successfully.\n", name);
}

static void cmdCreate(const char *name)
{
    if (!validName(name)) return;
    if (findChild(cwd, name)) { printf("Error: '%s' already exists.\n", name); return; }
    insertChild(cwd, newNode(name, 0, cwd));
    printf("File '%s' created successfully.\n", name);
}

static void cmdWrite(const char *name, const char *data)
{
    FileNode *f = findChild(cwd, name);
    if (!f)              
    { 
        printf("Error: file '%s' not found.\n", name); return;
    }
    if (f->isDirectory)  
    { 
        printf("Error: '%s' is a directory.\n", name); 
        return; 
    }
    int len = (int)strlen(data);
    int needed = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (needed > freeCount + f->blockCount) 
    {
        printf("Error: disk full. Not enough free blocks (need %d, available %d).\n",needed, freeCount + f->blockCount);
        return;
    }
    releaseFileBlocks(f);
    if (needed > 0) 
    {
        f->blockPointers = (int *)malloc(sizeof(int) * needed);
        if (!f->blockPointers) 
        { 
            fprintf(stderr, "Out of memory\n"); exit(1); 
        }
    }
    for (int i = 0; i < needed; i++) 
    {
        int idx = allocBlock();                      
        f->blockPointers[i] = idx;
        int chunk = len - i * BLOCK_SIZE;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;
        memcpy(virtualDisk[idx], data + i * BLOCK_SIZE, chunk);
    }
    f->blockCount = needed;
    f->size = len;
    printf("Data written successfully (size=%d bytes).\n", len);
}
static void cmdRead(const char *name)
{
    FileNode *f = findChild(cwd, name);
    if (!f)             
    { 
        printf("Error: file '%s' not found.\n", name); 
        return; 
    }
    if (f->isDirectory)  
    { 
        printf("Error: '%s' is a directory.\n", name); 
        return; 
    }
    if (f->size == 0)    
    { 
        printf("(empty)\n"); 
        return; 
    }
    int remaining = f->size;
    for (int i = 0; i < f->blockCount; i++) 
    {
        int chunk = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        fwrite(virtualDisk[f->blockPointers[i]], 1, chunk, stdout);
        remaining -= chunk;
    }
    printf("\n");
}

static void cmdDelete(const char *name)
{
    FileNode *f = findChild(cwd, name);
    if (!f)              
    { 
        printf("Error: file '%s' not found.\n", name); 
        return; 
    }
    if (f->isDirectory){ 
        printf("Error: '%s' is a directory. Use rmdir.\n", name); 
        return;}
    releaseFileBlocks(f);
    removeChild(cwd, f);
    free(f);
    printf("File deleted successfully.\n");
}

static void cmdRmdir(const char *name)
{
    FileNode *d = findChild(cwd, name);
    if (!d)              
    {
        printf("Error: directory '%s' not found.\n", name); 
        return; 
    }
    if (!d->isDirectory) 
    { 
        printf("Error: '%s' is a file. Use delete.\n", name); 
        return;
    }
    if (d->child)        
    { 
        printf("Error: directory '%s' is not empty.\n", name); 
        return; 
    }
    removeChild(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

static void cmdLs(void)
{
    if (!cwd->child) {printf("(empty)\n"); return;}
    FileNode *cur = cwd->child;
    do 
    {
        printf("%s%s\n", cur->name, cur->isDirectory ? "/" : "");
        cur = cur->next;
    } while (cur != cwd->child);
}

static void cmdCd(const char *name)
{
    char path[LINE_LEN];
    if (strcmp(name, "/") == 0) 
    {
        cwd = root;
    } 
    else if (strcmp(name, ".") == 0) 
    {
        
    } 
    else if (strcmp(name, "..") == 0) 
    {
        if (cwd != root) cwd = cwd->parent;
    } 
    else
    {
        FileNode *d = findChild(cwd, name);
        if (!d) {printf("Error: directory '%s' not found.\n", name); return;}
        if (!d->isDirectory) {printf("Error: '%s' is not a directory.\n", name); return;}
        cwd = d;
    }
    buildPath(cwd, path);
    printf("Moved to %s\n", path);
}

static void cmdPwd(void)
{
    char path[LINE_LEN];
    buildPath(cwd, path);
    printf("%s\n", path);
}

static void cmdDf(void)
{
    int used = numBlocks - freeCount;
    printf("Total Blocks: %d\n", numBlocks);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", 100.0 * used / numBlocks);
}
static char *skipWs(char *s)
{
    while (*s == ' ' || *s == '\t') s++;
    return s;
}
static char *nextToken(char *s, char *out, size_t outSize)
{
    s = skipWs(s);
    size_t i = 0;
    while (*s && *s != ' ' && *s != '\t') 
    {
        if (i < outSize - 1) out[i++] = *s;
        s++;
    }
    out[i] = '\0';
    return s;
}
static void handleWrite(char *rest)
{
    char name[LINE_LEN];
    rest = nextToken(rest, name, sizeof(name));
    rest = skipWs(rest);
    if (name[0] == '\0' || *rest == '\0') 
    {
        printf("Usage: write <filename> \"content\"\n");
        return;
    }
    if (*rest == '"') 
    {
        rest++;
        char *end = strrchr(rest, '"');
        if (end) *end = '\0';
    } 
    else 
    {
        size_t n = strlen(rest);
        while (n > 0 && (rest[n - 1] == ' ' || rest[n - 1] == '\t')) rest[--n] = '\0';
    }
    if (strlen(name) > MAX_NAME) 
    {
        printf("Error: name must be 1-%d characters.\n", MAX_NAME);
        return;
    }
    cmdWrite(name, rest);
}

static void runShell(void)
{
    char line[LINE_LEN];
    while (1) 
    {
        char path[LINE_LEN];
        buildPath(cwd, path);
        printf("%s > ", path);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;   
        line[strcspn(line, "\r\n")] = '\0';
        char cmd[LINE_LEN], arg[LINE_LEN];
        char *rest = nextToken(line, cmd, sizeof(cmd));
        if (cmd[0] == '\0') continue;

        if (strcmp(cmd, "exit") == 0) 
        {
            break;
        } 
        else if (strcmp(cmd, "write") == 0)
        {
            handleWrite(rest);
        } 
        else if (strcmp(cmd, "ls") == 0) 
        {
            cmdLs();
        } 
        else if (strcmp(cmd, "pwd") == 0) 
        {
            cmdPwd();
        } 
        else if (strcmp(cmd, "df") == 0) 
        {
            cmdDf();
        } 
        else if (strcmp(cmd, "mkdir") == 0 || strcmp(cmd, "create") == 0 || strcmp(cmd, "read") == 0 || strcmp(cmd, "delete") == 0 ||
                 strcmp(cmd, "rmdir") == 0 || strcmp(cmd, "cd") == 0) 
        {
            nextToken(rest, arg, sizeof(arg));
            if (arg[0] == '\0') 
            {
                printf("Usage: %s <name>\n", cmd);
            } 
            else if (strcmp(cmd, "cd") != 0 && strcmp(cmd, "read") != 0 && strcmp(cmd, "delete") != 0 && strcmp(cmd, "rmdir") != 0 &&
                     strlen(arg) > MAX_NAME) 
            {
                printf("Error: name must be 1-%d characters.\n", MAX_NAME);
            } 
            else if (strcmp(cmd, "mkdir") == 0) cmdMkdir(arg);
            else if (strcmp(cmd, "create") == 0) cmdCreate(arg);
            else if (strcmp(cmd, "read") == 0) cmdRead(arg);
            else if (strcmp(cmd, "delete") == 0) cmdDelete(arg);
            else if (strcmp(cmd, "rmdir") == 0) cmdRmdir(arg);
            else cmdCd(arg);
        }
        else 
        {
            printf("Unknown command: %s\n", cmd);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1) 
    {
        int n = atoi(argv[1]);
        if (n >= 1 && n <= MAX_BLOCKS) numBlocks = n;
        else printf("Invalid block count; using %d.\n", DEFAULT_BLOCKS);
    }
    initDisk();
    root = newNode("/", 1, NULL);
    root->parent = root;
    cwd = root;
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    runShell();
    freeTree(root);
    root = cwd = NULL;
    destroyFreeList();
    printf("Memory released. Exiting program...\n");
    return 0;
}