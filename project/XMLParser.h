#if !defined( FILE_XMLPARSER_H )
#define FILE_XMLPARSER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Class define pascal type string ( string don't end with '0' 
//  but it length is defined in variable l ), string can have max
//  length up to 2^16 (65536) chars.
class CPascalStr{ 
	char *sz;
	public:
	char *s; int l;
	CPascalStr() { sz=NULL; }
	~CPascalStr() { if (sz) delete[] sz; sz=NULL;}
	CPascalStr &operator =(const CPascalStr &str) {s=str.s; l=str.l; sz=NULL; return *this;}
	void set(char *ps, int len) { s = ps; l = len; }
	int cmp(char *psz) {
		if ( l==(int)strlen(psz) ) return _strnicmp(s, psz, l); 
		return 1;
	}
	int cmp(CPascalStr *pcstr) { 
		if ( l==pcstr->l ) return _strnicmp(s, pcstr->s, l); 
		return 1;
	}
	char *psz() { if (sz) delete[] sz; sz=new char[l+1];
		sz[l] = 0; strncpy(sz, s, l); return sz; }
};


class CXMLParser
{
	#define MAX_ATTRIBUTES 128 

	char *m_pszXMLText, *m_psz;
	CPascalStr	*m_pId,			// Here is keep element id, 
										//  element can have up to 128 attributes.

					*m_pAttribs;	// Here are keep attributes names,
										//  and it's values ( name-it's value-name-it's value ... ).

	
	int			m_iNumAttribs,		// Count of all element's attributes.
					m_iNumAttribsLeft;	// Count of element's attributes left.

	// Current level, level starts (+1) with '<' ( or '<name>' )
	//  and ends (-1) with '/>' ( or '</name'> );
	int			m_iLevel;

	ParseElement( char **buffer, CPascalStr *opt, int *opts, int *level );
	
public:

	CXMLParser();
	~CXMLParser();

	// Open and read XML file to memory.
	BOOL OpenXMLFile( char *pszFileName );
	// Open and read XML file from ZIP to memory.
	BOOL OpenXMLFile( char *pszFileName, cArchive *pArchive );

	BOOL IsOpened();

	// Parse next element and it's attributes.
	// On return: pId is  element id
	//				  iLevel is current element level
	//  return 0 when OK, or id of error when ERROR
	int NextElement( CPascalStr **ppId, int *piLevel );
	
	// Parse next element's attribute, 
	//  On return: pAttribName is attrib name, and pAttribVal is attrib val.
	//  return numbers of left attributes, 
	//	 or 0 when no attributes left ( need call to NextElement() ).
	int NextAttrib( CPascalStr **ppAttribName, CPascalStr **ppAttribVal );

};

#endif // !defined( FILE_XMLPARSER_H )