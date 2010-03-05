#include "interface.h"
#include "common.h"
#include "settings.h"
#include "resource.h"

// defines
enum {
	TYPE_ALLIANCE,
	TYPE_HORDE,
	TYPE_BEASTS,
	TYPE_DEMONS,
	TYPE_DRAGONS,
	TYPE_ELEMENTALS,
	TYPE_GIANTS,
	TYPE_HUMANOIDS,
	TYPE_UNDEAD,
	TYPE_MACHINES,
	TYPE_SLIMES,
	TYPE_CRITTERS,
	TYPE_TREASURE,
	TYPE_OBJECTS,
	TYPE_HERBS,
	TYPE_MINERALS
};

#define TEXTURE_DIALOGFRAME              {(FLOAT)  0/256, (FLOAT)  0/256, (FLOAT)256/256, (FLOAT) 32/256}
#define TEXTURE_TREEVIEWFRAME            {(FLOAT)  0/256, (FLOAT) 32/256, (FLOAT)256/256, (FLOAT) 64/256}
#define TEXTURE_REDBUTTONUP              {(FLOAT)  0/256, (FLOAT) 64/256, (FLOAT)128/256, (FLOAT) 96/256}
#define TEXTURE_REDBUTTONDOWN            {(FLOAT)128/256, (FLOAT) 64/256, (FLOAT)256/256, (FLOAT) 96/256}
#define TEXTURE_REDBUTTONDISABLED        {(FLOAT)  0/256, (FLOAT) 96/256, (FLOAT)128/256, (FLOAT)128/256}
#define TEXTURE_TEXTBOX                  {(FLOAT)128/256, (FLOAT) 96/256, (FLOAT)256/256, (FLOAT)128/256}
#define TEXTURE_SCROLLLEFTUP             {(FLOAT)  0/256, (FLOAT)128/256, (FLOAT) 32/256, (FLOAT)160/256}
#define TEXTURE_SCROLLLEFTDOWN           {(FLOAT) 32/256, (FLOAT)128/256, (FLOAT) 64/256, (FLOAT)160/256}
#define TEXTURE_SCROLLRIGHTUP            {(FLOAT) 64/256, (FLOAT)128/256, (FLOAT) 96/256, (FLOAT)160/256}
#define TEXTURE_SCROLLRIGHTDOWN          {(FLOAT) 96/256, (FLOAT)128/256, (FLOAT)128/256, (FLOAT)160/256}
#define TEXTURE_CHECKBUTTON              {(FLOAT)128/256, (FLOAT)128/256, (FLOAT)160/256, (FLOAT)160/256}
#define TEXTURE_CHECK                    {(FLOAT)160/256, (FLOAT)128/256, (FLOAT)192/256, (FLOAT)160/256}
#define TEXTURE_BUTTONHIGHLIGHT          {(FLOAT)192/256, (FLOAT)128/256, (FLOAT)224/256, (FLOAT)160/256}
#define TEXTURE_TRANSPARENTBACKGROUND    {(FLOAT)224/256, (FLOAT)128/256, (FLOAT)256/256, (FLOAT)160/256}
#define TEXTURE_ALLIANCE                 {(FLOAT)  0/256, (FLOAT)160/256, (FLOAT) 32/256, (FLOAT)192/256}
#define TEXTURE_HORDE                    {(FLOAT) 32/256, (FLOAT)160/256, (FLOAT) 64/256, (FLOAT)192/256}
#define TEXTURE_BEASTS                   {(FLOAT) 64/256, (FLOAT)160/256, (FLOAT) 96/256, (FLOAT)192/256}
#define TEXTURE_DEMONS                   {(FLOAT) 96/256, (FLOAT)160/256, (FLOAT)128/256, (FLOAT)192/256}
#define TEXTURE_DRAGONS                  {(FLOAT)128/256, (FLOAT)160/256, (FLOAT)160/256, (FLOAT)192/256}
#define TEXTURE_ELEMENTALS               {(FLOAT)160/256, (FLOAT)160/256, (FLOAT)192/256, (FLOAT)192/256}
#define TEXTURE_GIANTS                   {(FLOAT)192/256, (FLOAT)160/256, (FLOAT)224/256, (FLOAT)192/256}
#define TEXTURE_HUMANOIDS                {(FLOAT)224/256, (FLOAT)160/256, (FLOAT)256/256, (FLOAT)192/256}
#define TEXTURE_UNDEAD                   {(FLOAT)  0/256, (FLOAT)192/256, (FLOAT) 32/256, (FLOAT)224/256}
#define TEXTURE_MACHINES                 {(FLOAT) 32/256, (FLOAT)192/256, (FLOAT) 64/256, (FLOAT)224/256}
#define TEXTURE_SLIMES                   {(FLOAT) 64/256, (FLOAT)192/256, (FLOAT) 96/256, (FLOAT)224/256}
#define TEXTURE_CRITTERS                 {(FLOAT) 96/256, (FLOAT)192/256, (FLOAT)128/256, (FLOAT)224/256}
#define TEXTURE_TREASURE                 {(FLOAT)128/256, (FLOAT)192/256, (FLOAT)160/256, (FLOAT)224/256}
#define TEXTURE_OBJECTS                  {(FLOAT)160/256, (FLOAT)192/256, (FLOAT)192/256, (FLOAT)224/256}
#define TEXTURE_HERBS                    {(FLOAT)192/256, (FLOAT)192/256, (FLOAT)224/256, (FLOAT)224/256}
#define TEXTURE_MINERALS                 {(FLOAT)224/256, (FLOAT)192/256, (FLOAT)256/256, (FLOAT)224/256}
#define TEXTURE_BLUEHIGHLIGHT            {(FLOAT)  0/256, (FLOAT)224/256, (FLOAT)128/256, (FLOAT)240/256}
#define TEXTURE_GREYHIGHLIGHT            {(FLOAT)  0/256, (FLOAT)240/256, (FLOAT)128/256, (FLOAT)256/256}
#define TEXTURE_SCROLLBARKNOB            {(FLOAT)128/256, (FLOAT)224/256, (FLOAT)144/256, (FLOAT)240/256}
#define TEXTURE_BLACK                    {(FLOAT)144/256, (FLOAT)224/256, (FLOAT)160/256, (FLOAT)240/256}
#define TEXTURE_WHITE                    {(FLOAT)144/256, (FLOAT)240/256, (FLOAT)160/256, (FLOAT)256/256}
#define TEXTURE_CLOSEUP                  {(FLOAT)160/256, (FLOAT)224/256, (FLOAT)176/256, (FLOAT)240/256}
#define TEXTURE_CLOSEDOWN                {(FLOAT)176/256, (FLOAT)224/256, (FLOAT)192/256, (FLOAT)240/256}
#define TEXTURE_EXPANDBUTTON             {(FLOAT)192/256, (FLOAT)224/256, (FLOAT)208/256, (FLOAT)240/256}
#define TEXTURE_COLLAPSEBUTTON           {(FLOAT)208/256, (FLOAT)224/256, (FLOAT)224/256, (FLOAT)240/256}
#define TEXTURE_SCROLLUPBUTTONUP         {(FLOAT)160/256, (FLOAT)240/256, (FLOAT)176/256, (FLOAT)256/256}
#define TEXTURE_SCROLLUPBUTTONDOWN       {(FLOAT)176/256, (FLOAT)240/256, (FLOAT)192/256, (FLOAT)256/256}
#define TEXTURE_SCROLLUPBUTTONDISABLED   {(FLOAT)192/256, (FLOAT)240/256, (FLOAT)208/256, (FLOAT)256/256}
#define TEXTURE_SCROLLDOWNBUTTONUP       {(FLOAT)208/256, (FLOAT)240/256, (FLOAT)224/256, (FLOAT)256/256}
#define TEXTURE_SCROLLDOWNBUTTONDOWN     {(FLOAT)224/256, (FLOAT)240/256, (FLOAT)240/256, (FLOAT)256/256}
#define TEXTURE_SCROLLDOWNBUTTONDISABLED {(FLOAT)240/256, (FLOAT)240/256, (FLOAT)256/256, (FLOAT)256/256}

// function prototypes
PCDXCtrlButton   Interface_CreateCheckButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlButton   Interface_CreateCloseButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlDialog   Interface_CreateDialogWindow(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PSZ pszCaption);
PCDXCtrlBase     Interface_CreateGPSWindow();
PCDXCtrlBase     Interface_CreateMainWindow();
PCDXCtrlTextbox  Interface_CreateNumberTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlButton   Interface_CreateRedButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PSZ pszCaption);
PCDXCtrlButton   Interface_CreateScrollLeftButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlButton   Interface_CreateScrollRightButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlStatic   Interface_CreateStatic(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, BOOL bFixedWidth, PSZ pszText);
PCDXCtrlTextbox  Interface_CreateTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwMaxLength);
PCDXCtrlButton   Interface_CreateTrackButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwType);
PCDXCtrlTreeview Interface_CreateTreeview(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
PCDXCtrlWindow   Interface_CreateWindowControl(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
// global variables
PCDXCtrlManager g_pManager = NULL;
BOOL g_bInit = FALSE;

// code start
PCDXCtrlButton Interface_CreateCheckButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trButton = TEXTURE_CHECKBUTTON;
	TEXTURERECT trCheck = TEXTURE_CHECK;
	PCDXCtrlButton pButton = new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trButton, &trCheck, &trButton, "");
	pButton->SetCheckable(TRUE);
	return pButton;
}

PCDXCtrlButton Interface_CreateCloseButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trUp = TEXTURE_CLOSEUP;
	TEXTURERECT trDown = TEXTURE_CLOSEDOWN;
	return new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trUp, &trDown, &trUp, "");
}

PCDXCtrlDialog Interface_CreateDialogWindow(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PSZ pszCaption)
{
	TEXTURERECT trFrame = TEXTURE_DIALOGFRAME;
	TEXTURERECT trBackground = TEXTURE_TRANSPARENTBACKGROUND;
	return new CDXCtrlDialog(dwId, dwX, dwY, dwWidth, dwHeight, 32, &trFrame, &trBackground, pszCaption);
}

PCDXCtrlBase Interface_CreateGPSWindow()
{
	DWORD dwMainPos;
	Settings_LoadGPSWindowPosition(&dwMainPos);

	g_pGPSWindow = Interface_CreateDialogWindow(IDD_GPS, LOWORD(dwMainPos), HIWORD(dwMainPos), 125, 100, "GPS");
	g_pGPSWindow->Hide();
	g_pGPSClose = Interface_CreateCloseButton(IDC_GPSCLOSE, 100, 7, 16, 16);
	g_pGPSWindow->AddChild(g_pGPSClose);
	g_pGPSWindow->AddChild(Interface_CreateStatic(0, 15, 22, 20, 14, TRUE, "X:"));
	g_pGPSWindow->AddChild(Interface_CreateStatic(0, 15, 36, 20, 14, TRUE, "Y:"));
	g_pGPSWindow->AddChild(Interface_CreateStatic(0, 15, 50, 20, 14, TRUE, "Z:"));
	g_pGPSWindow->AddChild(Interface_CreateStatic(0, 15, 64, 20, 14, TRUE, "S:"));
	g_pGPSWindow->AddChild(Interface_CreateStatic(0, 15, 78, 20, 14, TRUE, "M:"));

	g_pGPSXEdit = Interface_CreateStatic(0, 35, 22, 80, 14, TRUE, "");
	g_pGPSXEdit->SetColor(-1);
	g_pGPSWindow->AddChild(g_pGPSXEdit);
	g_pGPSYEdit = Interface_CreateStatic(0, 35, 36, 80, 14, TRUE, "");
	g_pGPSYEdit->SetColor(-1);
	g_pGPSWindow->AddChild(g_pGPSYEdit);
	g_pGPSZEdit = Interface_CreateStatic(0, 35, 50, 80, 14, TRUE, "");
	g_pGPSZEdit->SetColor(-1);
	g_pGPSWindow->AddChild(g_pGPSZEdit);
	g_pGPSSEdit = Interface_CreateStatic(0, 35, 64, 80, 14, TRUE, "");
	g_pGPSSEdit->SetColor(-1);
	g_pGPSWindow->AddChild(g_pGPSSEdit);
	g_pGPSMEdit = Interface_CreateStatic(0, 35, 78, 80, 14, TRUE, "");
	g_pGPSMEdit->SetColor(-1);
	g_pGPSWindow->AddChild(g_pGPSMEdit);

	return g_pGPSWindow;
}

PCDXCtrlBase Interface_CreateMainWindow()
{
	DWORD dwMainPos;
	Settings_LoadMainWindowPosition(&dwMainPos);

	// main window
	g_pMainWindow = Interface_CreateDialogWindow(         IDD_MAIN             ,  LOWORD(dwMainPos),  HIWORD(dwMainPos), 490, 315, "Bubba's Warcraft Hack v1.4");
	g_pMainWindow->Hide();
	g_pClose = Interface_CreateCloseButton(               IDC_CLOSE            , 465,   7,  16,  16);
	g_pMainWindow->AddChild(g_pClose);

	// location list
	g_pFileEdit = Interface_CreateTextbox(                IDC_FILEEDIT         ,  14,  25, 175,  20, 20);
	g_pMainWindow->AddChild(g_pFileEdit);
	g_pLoad = Interface_CreateRedButton(                  IDC_LOAD             , 190,  25,  65,  20, "Load");
	g_pLoad->SetCheckable(TRUE);
	g_pMainWindow->AddChild(g_pLoad);
	g_pTreeview = Interface_CreateTreeview(               IDC_TREEVIEW         ,  14,  44, 241, 200);
	g_pMainWindow->AddChild(g_pTreeview);

	// control panel
	g_pNewGroup = Interface_CreateRedButton(              IDC_NEWGROUP         ,  14, 242,  80,  20, "New Group");
	g_pMainWindow->AddChild(g_pNewGroup);
	g_pAddLocation = Interface_CreateRedButton(           IDC_ADDLOCATION      ,  14, 262,  80,  20, "Add Location");
	g_pMainWindow->AddChild(g_pAddLocation);
	g_pEditEntry = Interface_CreateRedButton(             IDC_EDITENTRY        ,  14, 282,  80,  20, "Edit Entry");
	g_pMainWindow->AddChild(g_pEditEntry);
	g_pDeleteEntry = Interface_CreateRedButton(           IDC_DELETEENTRY      ,  94, 242,  80,  20, "Delete Entry");
	g_pMainWindow->AddChild(g_pDeleteEntry);
	g_pMoveUp = Interface_CreateRedButton(                IDC_MOVEUP           ,  94, 262,  80,  20, "Move Up");
	g_pMainWindow->AddChild(g_pMoveUp);
	g_pMoveDown = Interface_CreateRedButton(              IDC_MOVEDOWN         ,  94, 282,  80,  20, "Move Down");
	g_pMainWindow->AddChild(g_pMoveDown);
	g_pTeleport = Interface_CreateRedButton(              IDC_TELEPORT         , 174, 242,  80,  20, "Teleport");
	g_pMainWindow->AddChild(g_pTeleport);
	g_pGoToCorpse = Interface_CreateRedButton(            IDC_GOTOCORPSE       , 174, 262,  80,  20, "Go to Corpse");
	g_pMainWindow->AddChild(g_pGoToCorpse);
	g_pGPS = Interface_CreateRedButton(                   IDC_GPS              , 174, 282,  80,  20, "GPS");
	g_pMainWindow->AddChild(g_pGPS);

	// hack check buttons
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 108,  95,  20, FALSE, "Disable Hotkeys"));
	g_pDisableHotkeys = Interface_CreateCheckButton(      IDC_DISABLEHOTKEYS   , 259, 108,  20,  20);
	g_pMainWindow->AddChild(g_pDisableHotkeys);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 130,  95,  20, FALSE, "No Fall Damage"));
	g_pNoFallDamage = Interface_CreateCheckButton(        IDC_NOFALLDAMAGE     , 259, 130,  20,  20);
	g_pMainWindow->AddChild(g_pNoFallDamage);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 152,  95,  20, FALSE, "Mountain Climb"));
	g_pMountainClimb = Interface_CreateCheckButton(       IDC_MOUNTAINCLIMB    , 259, 152,  20,  20);
	g_pMainWindow->AddChild(g_pMountainClimb);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 174,  95,  20, FALSE, "Lock Speed"));
	g_pLockSpeed = Interface_CreateCheckButton(           IDC_LOCKSPEED        , 259, 174,  20,  20);
	g_pMainWindow->AddChild(g_pLockSpeed);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 196,  95,  20, FALSE, "Zero Gravity"));
	g_pZeroGravity = Interface_CreateCheckButton(         IDC_ZEROGRAVITY      , 259, 196,  20,  20);
	g_pMainWindow->AddChild(g_pZeroGravity);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 218,  95,  20, FALSE, "Teleport to Plane"));
	g_pTeleportToPlane = Interface_CreateCheckButton(     IDC_TELEPORTTOPLANE  , 259, 218,  20,  20);
	g_pMainWindow->AddChild(g_pTeleportToPlane);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 287, 240,  95,  20, FALSE, "Follow NPC"));
	g_pFollowNPC = Interface_CreateCheckButton(           IDC_FOLLOWNPC        , 259, 240,  20,  20);
	g_pMainWindow->AddChild(g_pFollowNPC);

	// position controls
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 259,  25,  20,  20, TRUE, "X:"));
	g_pXLeft = Interface_CreateScrollLeftButton(          IDC_XLEFT            , 276,  25,  20,  20);
	g_pMainWindow->AddChild(g_pXLeft);
	g_pXRight = Interface_CreateScrollRightButton(        IDC_XRIGHT           , 296,  25,  20,  20);
	g_pMainWindow->AddChild(g_pXRight);
	g_pXEdit = Interface_CreateNumberTextbox(             IDC_XEDIT            , 318,  25,  90,  20);
	g_pMainWindow->AddChild(g_pXEdit);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 259,  45,  20,  20, TRUE, "Y:"));
	g_pYLeft = Interface_CreateScrollLeftButton(          IDC_YLEFT            , 276,  45,  20,  20);
	g_pMainWindow->AddChild(g_pYLeft);
	g_pYRight = Interface_CreateScrollRightButton(        IDC_YRIGHT           , 296,  45,  20,  20);
	g_pMainWindow->AddChild(g_pYRight);
	g_pYEdit = Interface_CreateNumberTextbox(             IDC_YEDIT            , 318,  45,  90,  20);
	g_pMainWindow->AddChild(g_pYEdit);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 259,  65,  20,  20, TRUE, "Z:"));
	g_pZLeft = Interface_CreateScrollLeftButton(          IDC_ZLEFT            , 276,  65,  20,  20);
	g_pMainWindow->AddChild(g_pZLeft);
	g_pZRight = Interface_CreateScrollRightButton(        IDC_ZRIGHT           , 296,  65,  20,  20);
	g_pMainWindow->AddChild(g_pZRight);
	g_pZEdit = Interface_CreateNumberTextbox(             IDC_ZEDIT            , 318,  65,  90,  20);
	g_pMainWindow->AddChild(g_pZEdit);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 259,  85,  20,  20, TRUE, "S:"));
	g_pSLeft = Interface_CreateScrollLeftButton(          IDC_SLEFT            , 276,  85,  20,  20);
	g_pMainWindow->AddChild(g_pSLeft);
	g_pSRight = Interface_CreateScrollRightButton(        IDC_SRIGHT           , 296,  85,  20,  20);
	g_pMainWindow->AddChild(g_pSRight);
	g_pSEdit = Interface_CreateNumberTextbox(             IDC_SEDIT            , 318,  85,  90,  20);
	g_pMainWindow->AddChild(g_pSEdit);

	// track buttons
	g_pAlliance = Interface_CreateTrackButton(            IDC_ALLIANCE         , 413,  25,  32,  32, TYPE_ALLIANCE);
	g_pMainWindow->AddChild(g_pAlliance);
	g_pHorde = Interface_CreateTrackButton(               IDC_HORDE            , 445,  25,  32,  32, TYPE_HORDE);
	g_pMainWindow->AddChild(g_pHorde);
	g_pBeasts = Interface_CreateTrackButton(              IDC_BEASTS           , 413,  57,  32,  32, TYPE_BEASTS);
	g_pMainWindow->AddChild(g_pBeasts);
	g_pDemons = Interface_CreateTrackButton(              IDC_DEMONS           , 445,  57,  32,  32, TYPE_DEMONS);
	g_pMainWindow->AddChild(g_pDemons);
	g_pDragons = Interface_CreateTrackButton(             IDC_DRAGONS          , 413,  89,  32,  32, TYPE_DRAGONS);
	g_pMainWindow->AddChild(g_pDragons);
	g_pElementals = Interface_CreateTrackButton(          IDC_ELEMENTALS       , 445,  89,  32,  32, TYPE_ELEMENTALS);
	g_pMainWindow->AddChild(g_pElementals);
	g_pGiants = Interface_CreateTrackButton(              IDC_GIANTS           , 413, 121,  32,  32, TYPE_GIANTS);
	g_pMainWindow->AddChild(g_pGiants);
	g_pHumanoids = Interface_CreateTrackButton(           IDC_HUMANOIDS        , 445, 121,  32,  32, TYPE_HUMANOIDS);
	g_pMainWindow->AddChild(g_pHumanoids);
	g_pUndead = Interface_CreateTrackButton(              IDC_UNDEAD           , 413, 153,  32,  32, TYPE_UNDEAD);
	g_pMainWindow->AddChild(g_pUndead);
	g_pMachines = Interface_CreateTrackButton(            IDC_MACHINES         , 445, 153,  32,  32, TYPE_MACHINES);
	g_pMainWindow->AddChild(g_pMachines);
	g_pSlimes = Interface_CreateTrackButton(              IDC_SLIMES           , 413, 185,  32,  32, TYPE_SLIMES);
	g_pMainWindow->AddChild(g_pSlimes);
	g_pCritters = Interface_CreateTrackButton(            IDC_CRITTERS         , 445, 185,  32,  32, TYPE_CRITTERS);
	g_pMainWindow->AddChild(g_pCritters);
	g_pTreasure = Interface_CreateTrackButton(            IDC_TREASURE         , 413, 217,  32,  32, TYPE_TREASURE);
	g_pMainWindow->AddChild(g_pTreasure);
	g_pObjects = Interface_CreateTrackButton(             IDC_OBJECTS          , 445, 217,  32,  32, TYPE_OBJECTS);
	g_pMainWindow->AddChild(g_pObjects);
	g_pHerbs = Interface_CreateTrackButton(               IDC_HERBS            , 413, 249,  32,  32, TYPE_HERBS);
	g_pMainWindow->AddChild(g_pHerbs);
	g_pMinerals = Interface_CreateTrackButton(            IDC_MINERALS         , 445, 249,  32,  32, TYPE_MINERALS);
	g_pMainWindow->AddChild(g_pMinerals);
	g_pMainWindow->AddChild(Interface_CreateStatic(       0                    , 262, 261, 130,  20, TRUE, "Track Pattern:"));
	g_pTrackEdit = Interface_CreateTextbox(               IDC_TRACKEDIT        , 259, 281, 218,  20, 26);
	g_pMainWindow->AddChild(g_pTrackEdit);

	// new group window
	g_pNewGroupWindow = Interface_CreateWindowControl(    IDC_NEWGROUPWINDOW   ,  14,  44, 241, 200);
	g_pNewGroupWindow->Hide();
	g_pNewGroupWindow->AddChild(Interface_CreateStatic(   0                    ,  10,  10,  40,  20, TRUE, "Name:"));
	g_pNewGroupNEdit = Interface_CreateTextbox(           IDC_NEWGROUPNEDIT    ,  55,  10, 175,  20, 20);
	g_pNewGroupWindow->AddChild(g_pNewGroupNEdit);
	g_pNewGroupOk = Interface_CreateRedButton(            IDC_NEWGROUPOK       ,  30, 165,  80,  20, "Ok");
	g_pNewGroupWindow->AddChild(g_pNewGroupOk);
	g_pNewGroupCancel = Interface_CreateRedButton(        IDC_NEWGROUPCANCEL   , 130, 165,  80,  20, "Cancel");
	g_pNewGroupWindow->AddChild(g_pNewGroupCancel);
	g_pMainWindow->AddChild(g_pNewGroupWindow);

	// add location window
	g_pAddLocationWindow = Interface_CreateWindowControl( IDC_ADDLOCATIONWINDOW,  14,  44, 241, 200);
	g_pAddLocationWindow->Hide();
	g_pAddLocationWindow->AddChild(Interface_CreateStatic(0                    ,  10,  10,  40,  20, TRUE, "Name:"));
	g_pAddLocationWindow->AddChild(Interface_CreateStatic(0                    ,  10,  35,  40,  20, TRUE, "   X:"));
	g_pAddLocationWindow->AddChild(Interface_CreateStatic(0                    ,  10,  60,  40,  20, TRUE, "   Y:"));
	g_pAddLocationWindow->AddChild(Interface_CreateStatic(0                    ,  10,  85,  40,  20, TRUE, "   Z:"));
	g_pAddLocationWindow->AddChild(Interface_CreateStatic(0                    ,  10, 110,  40,  20, TRUE, " Map:"));
	g_pAddLocationNEdit = Interface_CreateTextbox(        IDC_ADDLOCATIONNEDIT ,  55,  10, 175,  20, 20);
	g_pAddLocationWindow->AddChild(g_pAddLocationNEdit);
	g_pAddLocationXEdit = Interface_CreateNumberTextbox(  IDC_ADDLOCATIONXEDIT ,  55,  35,  90,  20);
	g_pAddLocationWindow->AddChild(g_pAddLocationXEdit);
	g_pAddLocationYEdit = Interface_CreateNumberTextbox(  IDC_ADDLOCATIONYEDIT ,  55,  60,  90,  20);
	g_pAddLocationWindow->AddChild(g_pAddLocationYEdit);
	g_pAddLocationZEdit = Interface_CreateNumberTextbox(  IDC_ADDLOCATIONZEDIT ,  55,  85,  90,  20);
	g_pAddLocationWindow->AddChild(g_pAddLocationZEdit);
	g_pAddLocationMEdit = Interface_CreateNumberTextbox(  IDC_ADDLOCATIONMEDIT ,  55, 110,  90,  20);
	g_pAddLocationWindow->AddChild(g_pAddLocationMEdit);
	g_pAddLocationOk = Interface_CreateRedButton(         IDC_ADDLOCATIONOK    ,  30, 165,  80,  20, "Ok");
	g_pAddLocationWindow->AddChild(g_pAddLocationOk);
	g_pAddLocationCancel = Interface_CreateRedButton(     IDC_ADDLOCATIONCANCEL, 130, 165,  80,  20, "Cancel");
	g_pAddLocationWindow->AddChild(g_pAddLocationCancel);
	g_pMainWindow->AddChild(g_pAddLocationWindow);

	return g_pMainWindow;
}

PCDXCtrlTextbox Interface_CreateNumberTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trTextbox = TEXTURE_TEXTBOX;
	TEXTURERECT trCaret = TEXTURE_WHITE;
	PCDXCtrlTextbox pTextbox = new CDXCtrlTextbox(dwId, dwX, dwY, dwWidth, dwHeight, 10, &trTextbox, &trCaret);
	pTextbox->SetNumbersOnly(TRUE);
	return pTextbox;
}

PCDXCtrlButton Interface_CreateRedButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PSZ pszCaption)
{
	TEXTURERECT trUp = TEXTURE_REDBUTTONUP;
	TEXTURERECT trDown = TEXTURE_REDBUTTONDOWN;
	TEXTURERECT trDisabled = TEXTURE_REDBUTTONDISABLED;
	return new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trUp, &trDown, &trDisabled, pszCaption);
}

PCDXCtrlButton Interface_CreateScrollLeftButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trUp = TEXTURE_SCROLLLEFTUP;
	TEXTURERECT trDown = TEXTURE_SCROLLLEFTDOWN;
	PCDXCtrlButton pButton = new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trUp, &trDown, &trDown, "");
	pButton->SetRepeatable(TRUE);
	return pButton;
}

PCDXCtrlButton Interface_CreateScrollRightButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trUp = TEXTURE_SCROLLRIGHTUP;
	TEXTURERECT trDown = TEXTURE_SCROLLRIGHTDOWN;
	PCDXCtrlButton pButton = new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trUp, &trDown, &trDown, "");
	pButton->SetRepeatable(TRUE);
	return pButton;
}

PCDXCtrlStatic Interface_CreateStatic(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, BOOL bFixedWidth, PSZ pszText)
{
	PCDXCtrlStatic pStatic = new CDXCtrlStatic(dwId, dwX, dwY, dwWidth, dwHeight, bFixedWidth, pszText);
	pStatic->SetAlignment(DT_VCENTER | DT_LEFT);
	return pStatic;
}

PCDXCtrlTextbox Interface_CreateTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwMaxLength)
{
	TEXTURERECT trTextbox = TEXTURE_TEXTBOX;
	TEXTURERECT trCaret = TEXTURE_WHITE;
	return new CDXCtrlTextbox(dwId, dwX, dwY, dwWidth, dwHeight, dwMaxLength, &trTextbox, &trCaret);
}

PCDXCtrlButton Interface_CreateTrackButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwType)
{
	TEXTURERECT trAlliance = TEXTURE_ALLIANCE;
	TEXTURERECT trHorde = TEXTURE_HORDE;
	TEXTURERECT trBeasts = TEXTURE_BEASTS;
	TEXTURERECT trDemons = TEXTURE_DEMONS;
	TEXTURERECT trDragons = TEXTURE_DRAGONS;
	TEXTURERECT trElementals = TEXTURE_ELEMENTALS;
	TEXTURERECT trGiants = TEXTURE_GIANTS;
	TEXTURERECT trHumanoids = TEXTURE_HUMANOIDS;
	TEXTURERECT trUndead = TEXTURE_UNDEAD;
	TEXTURERECT trMachines = TEXTURE_MACHINES;
	TEXTURERECT trSlimes = TEXTURE_SLIMES;
	TEXTURERECT trCritters = TEXTURE_CRITTERS;
	TEXTURERECT trTreasure = TEXTURE_TREASURE;
	TEXTURERECT trObjects = TEXTURE_OBJECTS;
	TEXTURERECT trHerbs = TEXTURE_HERBS;
	TEXTURERECT trMinerals = TEXTURE_MINERALS;
	TEXTURERECT trHighlight = TEXTURE_BUTTONHIGHLIGHT;
	TEXTURERECT trButton;
	switch(dwType)
	{
	case TYPE_ALLIANCE:
		trButton = trAlliance;
		break;
	case TYPE_HORDE:
		trButton = trHorde;
		break;
	case TYPE_BEASTS:
		trButton = trBeasts;
		break;
	case TYPE_DEMONS:
		trButton = trDemons;
		break;
	case TYPE_DRAGONS:
		trButton = trDragons;
		break;
	case TYPE_ELEMENTALS:
		trButton = trElementals;
		break;
	case TYPE_GIANTS:
		trButton = trGiants;
		break;
	case TYPE_HUMANOIDS:
		trButton = trHumanoids;
		break;
	case TYPE_UNDEAD:
		trButton = trUndead;
		break;
	case TYPE_MACHINES:
		trButton = trMachines;
		break;
	case TYPE_SLIMES:
		trButton = trSlimes;
		break;
	case TYPE_CRITTERS:
		trButton = trCritters;
		break;
	case TYPE_TREASURE:
		trButton = trTreasure;
		break;
	case TYPE_OBJECTS:
		trButton = trObjects;
		break;
	case TYPE_HERBS:
		trButton = trHerbs;
		break;
	case TYPE_MINERALS:
		trButton = trMinerals;
		break;
	}
	PCDXCtrlButton pButton = new CDXCtrlButton(dwId, dwX, dwY, dwWidth, dwHeight, &trButton, &trHighlight, &trButton, "");
	pButton->SetCheckable(TRUE);
	return pButton;
}

PCDXCtrlTreeview Interface_CreateTreeview(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trFrame = TEXTURE_TREEVIEWFRAME;
	TEXTURERECT trBackground = TEXTURE_BLACK;
	TEXTURERECT trItemHighlight = TEXTURE_BLUEHIGHLIGHT;
	TEXTURERECT trGroupHighlight = TEXTURE_GREYHIGHLIGHT;
	TEXTURERECT trCollapse = TEXTURE_COLLAPSEBUTTON;
	TEXTURERECT trExpand = TEXTURE_EXPANDBUTTON;
	TEXTURERECT trScrollUpUp = TEXTURE_SCROLLUPBUTTONUP;
	TEXTURERECT trScrollUpDown = TEXTURE_SCROLLUPBUTTONDOWN;
	TEXTURERECT trScrollUpDisabled = TEXTURE_SCROLLUPBUTTONDISABLED;
	TEXTURERECT trScrollDownUp = TEXTURE_SCROLLDOWNBUTTONUP;
	TEXTURERECT trScrollDownDown = TEXTURE_SCROLLDOWNBUTTONDOWN;
	TEXTURERECT trScrollDownDisabled = TEXTURE_SCROLLDOWNBUTTONDISABLED;
	TEXTURERECT trKnob = TEXTURE_SCROLLBARKNOB;
	return new CDXCtrlTreeview(dwId, dwX, dwY, dwWidth, dwHeight, 16, &trFrame, &trBackground, &trItemHighlight, &trGroupHighlight, &trCollapse, &trExpand, &trScrollUpUp, &trScrollUpDown, &trScrollUpDisabled, &trScrollDownUp, &trScrollDownDown, &trScrollDownDisabled, &trKnob, TRUE);
}

PCDXCtrlWindow Interface_CreateWindowControl(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	TEXTURERECT trFrame = TEXTURE_DIALOGFRAME;
	TEXTURERECT trBackground = TEXTURE_TRANSPARENTBACKGROUND;
	return new CDXCtrlWindow(dwId, dwX, dwY, dwWidth, dwHeight, 16, &trFrame, &trBackground);
}

VOID Interface_DeleteControls()
{
	delete g_pManager;
}

VOID Interface_FreeWindow()
{
	g_pManager->FreeResources();
	g_pManager->FreeD3DWindow();
}

PCDXCtrlManager Interface_InitControls(HWND hwndWindow, HMODULE hModule, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold)
{
	PVOID pvAtlas;
	DWORD dwAtlasSize;

	g_pManager = new CDXCtrlManager(300, pszFixedFont, iFixedSize, bFixedBold, pszProportionalFont, iProportionalSize, bProportionalBold);
	if(g_pManager == NULL)
		return NULL;

	pvAtlas = ManualLoadDataResource(hModule, IDR_ATLAS, &dwAtlasSize);
	g_pManager->SetTextureAtlas(pvAtlas, dwAtlasSize);
	Interface_SetWindow(hwndWindow);

	g_pManager->AddControl(Interface_CreateMainWindow());
	g_pManager->AddControl(Interface_CreateGPSWindow());

	return g_pManager;
}

VOID __stdcall Interface_PresentHook(PDIRECT3DDEVICE9 pD3DDevice)
{
	if(g_pManager->GetD3DDevice() != pD3DDevice)
	{
		g_pManager->SetD3DDevice(pD3DDevice);
		if(g_bInit == FALSE)
		{
			g_bInit = TRUE;
			LoadSavedState();
		}
	}

	g_pManager->Render();
}

VOID __stdcall Interface_ResetHook()
{
	g_pManager->FreeResources();
}

VOID Interface_SetWindow(HWND hwndWindow)
{
	g_pManager->SetD3DWindow(hwndWindow);
}