
#define ID_ACCEL 16384

#define KEY_PLAYPAUSE				ID_ACCEL			// Space
#define KEY_STOP						ID_ACCEL+1		// V
#define KEY_REWINDBACK				ID_ACCEL+2		// Left
#define KEY_REWINDFORWARD			ID_ACCEL+3		// Right
#define KEY_VOLUMEUP					ID_ACCEL+4		// Up
#define KEY_VOLUMEDOWN				ID_ACCEL+5		// Down
#define KEY_GOFULLSCREEN			ID_ACCEL+6		// Alt+Enter
#define KEY_GOWINDOWED				ID_ACCEL+7		// Esc
#define KEY_EXIT						ID_ACCEL+8		// Alt+F4
#define KEY_PANSCAN					ID_ACCEL+9		// P
#define KEY_PREVMOVIE				ID_ACCEL+10		// Z
#define KEY_NEXTMOVIE				ID_ACCEL+11		// X
#define KEY_SUBTITLES				ID_ACCEL+12		// S
#define KEY_OPENMOVIE				ID_ACCEL+13		// Ctrl+O
#define KEY_OPENSUBTITLES			ID_ACCEL+14		// Shift+O
#define KEY_OPTIONS					ID_ACCEL+15		// Ctrl+P
#define KEY_MOVIEPROP				ID_ACCEL+16		// Shift+p
#define KEY_SKINBROWSER				ID_ACCEL+17
#define KEY_PLAYLIST					ID_ACCEL+18		// L

#define KEY_PLSOPEN					ID_ACCEL+13 	// Ctrl+O
#define KEY_PLSSAVE					ID_ACCEL+101	// Ctrl+S
#define KEY_PLSCLEAR					ID_ACCEL+102	// Ctrl+C
#define KEY_PLSADDFILES				ID_ACCEL+103	// Ctrl+A
#define KEY_PLSDELETE				ID_ACCEL+104	// Delete
#define KEY_PLSCLOSE 				ID_ACCEL+7  	// Esc
#define KEY_PLSPLAY	 				ID_ACCEL+106	// Enter

extern ACCEL accel[];
char *GetKeyName( int keyId );
int GetKeysCount();