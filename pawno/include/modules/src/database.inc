/*
		Database loading
*/

#include "modules\header\database.txt"

new bool:DatabaseSetToReload = false;

forward ReloadDatabase();
public ReloadDatabase()
{
    sqliteconnection = db_open("AAD.db");
    DatabaseSetToReload = false;
    SendClientMessageToAll(-1, ""COL_PRIM"SQLite database has been reloaded successfully.");
	return 1;
}

SetDatabaseToReload(playerid = INVALID_PLAYER_ID)
{
	if(playerid != INVALID_PLAYER_ID)
		SendClientMessageToAll(-1, sprintf("{FFFFFF}%s "COL_PRIM"has set the SQLite database to reload.", Player[playerid][Name]));

	DatabaseSetToReload = true;
	db_close(sqliteconnection);
	SetTimer("ReloadDatabase", 100, false);
	return 1;
}

LoadDatabase()
{
	//db_close(sqliteconnection);
	sqliteconnection = db_open("AAD.db");
	//db_debug_openfiles();
	//db_debug_openresults();
	DatabaseLoading = true;
	SetTimer("DatabaseDefaultReload", 100, false);
	return 1;
}

forward DelayedDatabaseStuff();
public DelayedDatabaseStuff()
{
    // Vacuum SQL database
	db_free_result(db_query(sqliteconnection, "VACUUM"));

	LoadConfig(); // Loads server configuration
	LoadTextDraws(); // Loads all global textdraws
	LoadBases(); // Loads bases
	LoadArenas(); // Loads arenas
	LoadDMs(); // Loads DMs
	LoadCommands(); // Load commands with level
	
	DatabaseLoading = false;
    return 1;
}

forward DatabaseDefaultReload();
public DatabaseDefaultReload()
{
	if(sqliteconnection == DB:0)
		return 0;
	
    SetDatabaseToReload();
    SetTimer("DelayedDatabaseStuff", 200, false);
	return 1;
}

OptimizeDatabase()
{
	// Delete inactive accounts (3 days without logging in is considered as inactivity)
	new currYear, currMonth, currDay;
	getdate(currYear, currMonth, currDay);
	new str[150];
	format(str, sizeof str, "DELETE FROM Players WHERE ((LastSeen_Year = %d AND LastSeen_Month = %d AND LastSeen_Day + 3 < %d) OR LastSeen_Year != %d OR LastSeen_Month != %d) AND Level < 1", currYear, currMonth, currDay, currYear, currMonth);
	db_free_result(db_query(sqliteconnection, str));
	
	// Vacuum SQL database
	db_free_result(db_query(sqliteconnection, "VACUUM"));
}