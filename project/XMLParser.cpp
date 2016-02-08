
#include "stdafx.h"
#include "xmlparser.h"


CXMLParser::CXMLParser()
{
	m_pszXMLText = NULL; m_psz = NULL;
	m_pAttribs  = new CPascalStr[ MAX_ATTRIBUTES ];
}

CXMLParser::~CXMLParser()
{	
	if ( m_pszXMLText ) 
	{
		delete[] m_pszXMLText;
		m_pszXMLText = NULL; m_psz = NULL;
		
	}
	delete[] m_pAttribs;
}

BOOL CXMLParser::OpenXMLFile( char *pszFileName )
{
	cFile File;

	if ( m_pszXMLText ) 
	{
		delete[] m_pszXMLText;
		m_pszXMLText = NULL; m_psz = NULL;
		
	}
  
	if ( !File.Open( pszFileName, "rb") ) 
		return FALSE;

	DWORD dwFilesize = File.Size();
	if ( dwFilesize <= 0 ) return FALSE;

	m_pszXMLText = new char[ dwFilesize + 1 ];
	File.Read( m_pszXMLText, dwFilesize );

	m_psz = m_pszXMLText; m_psz[ dwFilesize ] = 0;
	m_iLevel = 0; m_iNumAttribs = 0; m_iNumAttribsLeft = 0;
	return TRUE;
}

BOOL CXMLParser::OpenXMLFile( char *pszFileName, cArchive *pArchive )
{
	if ( m_pszXMLText ) 
	{
		delete[] m_pszXMLText;
		m_pszXMLText = NULL; m_psz = NULL;
		
	}

	if ( !pArchive || !pArchive->IsOpenedArchive() ) return FALSE;

	DWORD dwFilesize = pArchive->SizeFile( pszFileName );
	if ( dwFilesize <= 0 ) return FALSE;

	m_pszXMLText = new char [ dwFilesize + 1 ];

	pArchive->ReadFile( pszFileName, (BYTE*)m_pszXMLText, &dwFilesize );

	m_psz = m_pszXMLText; m_psz[ dwFilesize ] = 0;
	m_iLevel = 0; m_iNumAttribs = 0; m_iNumAttribsLeft = 0;
	return TRUE;
}

BOOL CXMLParser::IsOpened()
{
	if ( !m_pszXMLText ) return FALSE;
	
	return TRUE;
}


int CXMLParser::ParseElement( char **buffer, CPascalStr *opt, int *opts, int *level )
{
	#define SKIP_ENDL_SPACES							\
		{while(*buff && *buff<=32 ) buff++;				\
		 if (!*buff) return 1; /*unexpected eof*/ }

	//XMLVERINFO looks: <? ... ?>
	#define SKIP_XMLVERINFO									\
		if (*buff=='?') {										\
			while (*++buff && *buff!='>') ;				\
			if (!*buff) return 1; /*unexpected eof*/	\
			*opts = 0; *level = 0; *buffer = ++buff;	\
			return 0; }				
	
	//COMMENT looks: <!-- ... -->
	#define SKIP_COMMENT													\
	if (*buff=='!') { buff++;											\
			if ( *buff++!='-' || *buff++!='-' ) return 3;		\
			do {																\
				if (*buff=='-') buff++;									\
			} while (*buff && *buff++!='-');							\
			if (!*buff || *buff!='>') return 3;						\
			*opts = 0; *level = 0; *buffer = ++buff;				\
			return 0; }														\


	#define PARSE_LABEL											\
		if(*buff && ((*buff>='a' && *buff<='z') ||				\
			(*buff>='A' && *buff<='Z') || *buff=='_' )) {		\
			while (*++buff && ((*buff>='a' && *buff<='z') ||	\
				  (*buff>='A' && *buff<='Z') ||					\
				  (*buff>='0' && *buff<='9') || *buff=='_' )) ;	\
			if (!*buff) return 1; /*unexpected eof*/ }			\
		else return 2; /*expected label*/
	
	#define PARSE_STRING									\
		if (*buff && *buff=='"') {							\
			while (*++buff && *buff<=126 && *buff!='"') ;	\
			if (!*buff) return 1; /*unexpected eof*/		\
			if (*buff>126)	return 3; /*inavlid char*/		\
			buff++;											\
		} else return 4; /*expected string*/

	char *ps, *buff = *buffer;
	*opts = 0; *level = 0;


	SKIP_ENDL_SPACES
	if (*buff++!='<') return 3; /*invalid char*/
	SKIP_ENDL_SPACES
	SKIP_XMLVERINFO
	SKIP_COMMENT
	if (*buff=='/') { 
		(*level)--; buff++;
		SKIP_ENDL_SPACES
	}
	ps = buff;
	PARSE_LABEL
	opt->set(ps, buff-ps); opt++; (*opts)++;
	if (*level == -1) {
		SKIP_ENDL_SPACES
		if (*buff=='>') {
			*buff++; *buffer = buff;
			return 0;
		}
		return 3; /*invalid char*/
	}

	while (*buff && *buff!='/' && *buff!='>')
	{
		SKIP_ENDL_SPACES
		ps = buff; PARSE_LABEL 
		opt->set(ps, buff-ps); opt++; (*opts)++;

		SKIP_ENDL_SPACES
		if (*buff++!='=') return 3; /*invalid char*/


		SKIP_ENDL_SPACES
		ps = buff; PARSE_STRING
		opt->set(ps+1, buff-ps-2); opt++; (*opts)++;

		SKIP_ENDL_SPACES
	}
	if (!*buff) return 1; /*unexpected eof*/
	if (*buff=='/') { buff++; (*level)--; SKIP_ENDL_SPACES}
	if (*buff++!='>') return 3; /*invalid char*/

	*buffer = buff; (*level)++;

	return 0;
}

int CXMLParser::NextElement( CPascalStr **ppId, int *piLevel )
{

	int ret, iLevelChange;
	while ( 
		!(ret = ParseElement( &m_psz, m_pAttribs, 
			&m_iNumAttribs, &iLevelChange ) ) 
		&& !m_iNumAttribs ) ;

	m_iLevel += iLevelChange;
	m_iNumAttribs = (m_iNumAttribs-1)/2;
	m_iNumAttribsLeft = m_iNumAttribs; 

	*ppId = m_pAttribs;
	*piLevel = m_iLevel;

	return ret;
}

int CXMLParser::NextAttrib( CPascalStr **ppAttribName, CPascalStr **ppAttribVal )
{
	if ( m_iNumAttribsLeft )
	{
		int off = (m_iNumAttribs-m_iNumAttribsLeft)*2 + 1;
		*ppAttribName = m_pAttribs+off;
		*ppAttribVal = m_pAttribs+off+1;

		m_iNumAttribsLeft--;

		return (m_iNumAttribsLeft+1);
	}

	return 0;
}
