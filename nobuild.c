#define NOBUILD_IMPLEMENTATION
#include <./nobuild.h>

int main(int argc, char** argv){
    GO_REBUILD_URSELF(argc,argv);
    MKDIRS("build");
    CMD("wget","-qO","temp.txt","https://ziglang.org/download/index.json");

    FILE* fout = fopen("temp.txt","r+");
    fseek(fout,0,SEEK_END);
    size_t file_size = ftell(fout);
    fseek(fout,0,SEEK_SET);
    char* contents = (char*)malloc(file_size);
    char buff[1024] = {0};
    char* pos = contents;
    while (fgets(buff, 1024, fout) != NULL) {
        strncat(pos,buff,1024);
    }

    cJSON* data = cJSON_Parse(contents);
    data = data->next == NULL ? data->child : data;
    while(data->next != NULL){
        char* name = data->string;
        if(strcmp(name,"master") != 0){
            break;
        }
        data = data->next;
    }
    cJSON* child = data->child;
    while(child != NULL){
        char* name = child->string;
        if(strcmp(name,"x86_64-linux") == 0){
            char* link = child->child->valuestring;
            CMD("wget",link);
            Cstr_Array split = SPLIT(link,"/");
            char* last_str = split.elems[split.count-1];
            // char foldername[260] = {0};
            // snprintf(foldername,260,"temp_dir%s%s",PATH_SEP,last_str);
            // size_t len = strlen(foldername);
            // while(foldername[len-1] != '.'){
            //     foldername[--len] = '\0';
            // }
            // foldername[len-1] = '\0';
            // strcat(foldername,"/");
            CMD("tar","-xf",last_str,"--strip-components=1","-C","FoundryTools_linux_x64");
            // CMD("rsync","-av",foldername,"FoundryTools_windows_x64");
            // CMD("rm","-rf","temp_dir");
            RM(last_str);
        }
        if(strcmp(name,"x86_64-windows") == 0){
            char* link = child->child->valuestring;
            CMD("wget",link);
            Cstr_Array split = SPLIT(link,"/");
            char* last_str = split.elems[split.count-1];
            char foldername[260] = {0};
            snprintf(foldername,260,"temp_dir%s%s",PATH_SEP,last_str);
            size_t len = strlen(foldername);
            while(foldername[len-1] != '.'){
                foldername[--len] = '\0';
            }
            foldername[len-1] = '\0';
            strcat(foldername,"/");
            CMD("unzip",last_str,"-d","temp_dir");
            CMD("rsync","-av",foldername,"FoundryTools_windows_x64");
            CMD("rm","-rf","temp_dir");
            RM(last_str);
        }
        child = child->next;
    }
    fclose(fout);
    RM("temp.txt");
}