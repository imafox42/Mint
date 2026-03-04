#include "Mint.h"

int main(){
    printf("| MINT SYSTEM TEST\n");

    printf("| RUNNING - init\n");
    int init = Mint_Initilize();
    if(init != 0){
        printf("| INIT FAILED ! | exit code : %d", init);
    }
}