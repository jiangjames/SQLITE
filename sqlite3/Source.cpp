#include "sqlite3helper.h"
#include <iostream>
#include <io.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <regex>
#include "FolderScanner.h"
using namespace SQLITE;
using namespace std;

void main(void)
{
    CFolderScanner scanner;
    const std::string filter("*.cpp");
    cout<<scanner.Init("C:\\project\\sqlite\\",&filter,1)<<endl;
    scanner.Start();
    UTILITY::SIZE_T size;
    scanner.GetFiles(size);
    cout<<"files:"<<size<<endl;
    return;
}