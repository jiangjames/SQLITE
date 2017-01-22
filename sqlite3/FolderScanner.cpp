#include "FolderScanner.h"
#include <io.h>
#include <sys/stat.h>
#include <regex>

static UTILITY::BOOL IsDirectory(const UTILITY::CHAR *path)
{
    struct stat st;
    if( (0 == stat(path, &st)) && (st.st_mode && S_IFDIR) && (st.st_dev != -1) )
        return UTILITY::TRUE;
    return UTILITY::FALSE;
}

static UTILITY::VOID ScanFolder(const UTILITY::CHAR *path, const std::string &filter, CFolderScanner::FilesContainer &files)
{
    std::string directory = path;
    directory += "*.*";
    struct _finddata_t fileAttr;
    UTILITY::INT handle = _findfirst(directory.c_str(),&fileAttr);
    if( -1 == handle )
        return;
    while (0 == _findnext(handle, &fileAttr))
    {
        if(fileAttr.attrib&_A_NORMAL)
            files.push_back(fileAttr.name);
        else if( (fileAttr.attrib&_A_SUBDIR) && IsDirectory(fileAttr.name))
            ScanFolder(fileAttr.name,filter,files);
    }
}

CFolderScanner::CFolderScanner()
{
}


CFolderScanner::~CFolderScanner()
{
}


UTILITY::BOOL   CFolderScanner::Init(const std::string &directory, const std::string filters[], UTILITY::SIZE_T filters_count)
{
    UTILITY_ASSERT( IsDirectory(directory.c_str()) );

    m_directory = directory;
    std::regex regexFilter("\\b\\\\*.", std::regex_constants::nosubs);
    while (filters_count--)
    {
        if (std::regex_search(filters[filters_count], regexFilter))
            m_Filters.insert(filters[filters_count]);
        else
            return UTILITY::FALSE;
    }
    return UTILITY::TRUE;
}

UTILITY::VOID CFolderScanner::Start(UTILITY::BOOL bReset /*= UTILITY::TRUE*/)
{
    bReset && Reset();

    for(FilterCstItor itor = m_Filters.begin(); itor != m_Filters.end(); ++itor)
        ScanFolder(m_directory.c_str(), *itor, m_Files);
}

const std::string * CFolderScanner::GetFiles(UTILITY::SIZE_T & size) const
{
    size = m_Files.size();

    if (m_Files.size())
        return &m_Files[0];
    return UTILITY_NULL;
}

UTILITY::BOOL   CFolderScanner::Reset()
{
    m_Files.clear();
    return UTILITY::FALSE;
}