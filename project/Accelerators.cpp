
#include "stdafx.h"
#include "accelerators.h"
#include "language.h"


ACCEL accel[] =  {
	FNOINVERT|FVIRTKEY, VK_SPACE, KEY_PLAYPAUSE,
	FNOINVERT|FVIRTKEY, 'V', KEY_STOP,
	FNOINVERT|FVIRTKEY, VK_LEFT, KEY_REWINDBACK,
	FNOINVERT|FVIRTKEY, VK_RIGHT, KEY_REWINDFORWARD,
	FNOINVERT|FVIRTKEY, VK_UP, KEY_VOLUMEUP,
	FNOINVERT|FVIRTKEY, VK_DOWN, KEY_VOLUMEDOWN,
	FNOINVERT|FVIRTKEY|FALT, VK_RETURN, KEY_GOFULLSCREEN,
	FNOINVERT|FVIRTKEY, VK_ESCAPE, KEY_GOWINDOWED,
	FNOINVERT|FVIRTKEY|FALT, VK_F4, KEY_EXIT,
	FNOINVERT|FVIRTKEY, 'P', KEY_PANSCAN,
	FNOINVERT|FVIRTKEY, 'Z', KEY_PREVMOVIE,
	FNOINVERT|FVIRTKEY, 'X', KEY_NEXTMOVIE,
	FNOINVERT|FVIRTKEY, 'S', KEY_SUBTITLES,
	FNOINVERT|FVIRTKEY|FCONTROL, 'O', KEY_OPENMOVIE,
	FNOINVERT|FVIRTKEY|FSHIFT, 'O', KEY_OPENSUBTITLES,
	FNOINVERT|FVIRTKEY|FCONTROL, 'P', KEY_OPTIONS,
	FNOINVERT|FVIRTKEY|FSHIFT, 'P', KEY_MOVIEPROP,

	FNOINVERT|FVIRTKEY, 'L', KEY_PLAYLIST,
	FNOINVERT|FVIRTKEY|FCONTROL, 'O', KEY_PLSOPEN,
	FNOINVERT|FVIRTKEY|FCONTROL, 'S', KEY_PLSSAVE,
	FNOINVERT|FVIRTKEY|FCONTROL, 'C', KEY_PLSCLEAR,
	FNOINVERT|FVIRTKEY|FCONTROL, 'A', KEY_PLSADDFILES,
	FNOINVERT|FVIRTKEY, VK_DELETE, KEY_PLSDELETE,
	FNOINVERT|FVIRTKEY, VK_ESCAPE, KEY_PLSCLOSE,
	FNOINVERT|FVIRTKEY, VK_RETURN, KEY_PLSPLAY

};

char *GetKeyName( int keyId )
{
	int numAccel = sizeof(accel) / sizeof(ACCEL);
	for ( int i=0; i<numAccel && keyId!=accel[i].cmd; i++ ) ;

	if ( i==numAccel ) return NULL;

	static char szkey[64];

	*szkey = 0;
	int flags = accel[i].fVirt&(FSHIFT|FCONTROL|FALT);
	while ( flags ) {
		int f=0;
		if ( flags&FSHIFT ) f=FSHIFT;
		else if ( flags&FCONTROL ) f=FCONTROL;
		else if ( flags&FALT ) f=FALT;
		else break;

		sprintf( szkey+strlen(szkey), "%s + ", transl(f, TRANSL_KEYS) );

		flags &= ~f;
	}

	char *psz = transl( accel[i].key, TRANSL_KEYS );
	if ( !psz || !*psz ) {
		char szAccelerator[16];
		int map = (accel[i].fVirt&FVIRTKEY)?MapVirtualKey(accel[i].key, 0):accel[i].key;
		GetKeyNameText( map<<16, szAccelerator, 16 );

		strcpy( szkey+strlen(szkey), szAccelerator );
	}
	else strcpy( szkey+strlen(szkey), psz );

	return szkey;
}

int GetKeysCount()
{
	return (sizeof(accel)/sizeof(ACCEL));
}