#if !defined( FILE_SKINDEFINITION_H )
#define FILE_SKIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//image definition
enum IMAGEID {
	IMAGE_BACKGROUND = 0,	//first image MUST be always IMAGE_BACKGROUND
	IMAGE_BUTTONS,
	IMAGE_BUTTONS_PRESS,
	IMAGE_BUTTOMS_OVER
};

//string id must be always STATIC
static char *imgId[] =  {
	"background", 
	"buttons", 
	"buttons.press", 
	"buttons.over"
};


//buttons definition
enum BUTTONID {
	BW_CLOSE = 0,
	BW_MINIMIZE,
	BW_MAXIMIZE,
	BW_SYSMENU,
	BW_RESIZE,
	BW_CHANGE_SKIN,
	BW_ABOUT,

	BP_PLAY,
	BP_PAUSE,
	BP_PLAY_PAUSE,
	BP_REWIND_FORWARD,
	BP_REWIND_BACK,
	BP_STOP,

	BP_NEXT_MOVIE,
	BP_PREV_MOVIE,

	BP_OPEN,
	BP_OPEN_SUBTITLES,
	BP_SUBTITLES_ON_OFF,
	BP_SUBTITLES_DELAY,
	BP_FULLSCREEN_WINDOWED,
	BP_MODE_NORMAL_COMPACT,
	BP_PANSCAN_ON_OFF,
	BP_PANSCAN_MENU,
	BP_VOLUME_ON_OFF,
	BP_PLAYLIST_SHOW_HIDE,
	BP_OPTIONS
};

static char *buttonId[] = {
	"window.close",								// 0
	"window.minimize",
	"window.maximize",
	"window.sysmenu",
	"window.resize",
	"window.change_skin",
	"window.about",

	"player.play",									// 7
	"player.pause",
	"player.play_pause",
	"player.rewindforward",
	"player.rewindback",
	"player.stop",

	"player.next_movie",							// 13
	"player.prev_movie",

	"player.open",									// 15
	"player.open_subtitles",
	"player.subtitles_on_off",
	"player.subtitles_delay",
	"player.fullscreen_windowed",
	"player.compactmode_on_off",
	"player.panscan_on_off",
	"player.panscan_menu",
	"player.volume_on_off",
	"player.playlist_show_hide",
	"player.options"								// 24

														// 25 buttons
};



//scrollbars definition
enum SCROLLBARID {
	SP_POSITION = 0,
	SP_VOLUME	
};

static char *scrollbarId[] = {
	"player.position", 
	"player.volume"
};

enum STATUSID {
	ST_MOVIESTATE = 0,
	ST_MOVIETIME
};

static char *statusId[] = {
	"movie.state",
	"movie.time"
};


#endif // !defined( FILE_SKINDEFINITION_H )