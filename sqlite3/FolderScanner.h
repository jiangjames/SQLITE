#pragma once


#include <set>
#include <vector>
#include "basetype.h"

class CFolderScanner{
public:
    typedef std::vector<std::string>         FilesContainer;
    typedef FilesContainer::iterator         FilesItor;
    typedef FilesContainer::const_iterator   FilesCstItor;

    typedef std::set<std::string>            FilterContainer;
    typedef FilterContainer::iterator        FilterItor;
    typedef FilterContainer::const_iterator  FilterCstItor;

public:
    CFolderScanner();
    virtual ~CFolderScanner();
    UTILITY::BOOL   Init(const std::string &directory, const std::string filters[], UTILITY::SIZE_T filters_count);
    UTILITY::VOID   Start(UTILITY::BOOL bReset = UTILITY::TRUE);
    const std::string*   GetFiles(UTILITY::SIZE_T &size) const;
    UTILITY::BOOL        Reset();

private:
    CFolderScanner(const CFolderScanner&);
    CFolderScanner& operator=(const CFolderScanner&);

private:
    std::string     m_directory;
    FilesContainer  m_Files;
    FilterContainer m_Filters;
};

