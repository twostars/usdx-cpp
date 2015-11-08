#include "stdafx.h"
#include "UsdxDatabase.h"
#include "Log.h"
#include <sqlite/sqlite3.h>

/*
	cDBVersion - history
	0 = USDX 1.01 or no database
	01 = USDX 1.1
*/
const int cDBVersion = 01; // 0.1
static const char cUS_Scores[] = "us_scores";
static const char cUS_Songs[] = "us_songs";
static const char * cUS_Statistics_Info = "us_statistics_info";

void UsdxDatabase::Initialize()
{
	// Add table cUS_Statistics_Info
	// needed in the conversion from 1.01 to 1.1
	if (!TableExists(cUS_Statistics_Info))
	{
		sLog.Info("Database::Init", "Outdated song database found - missing table %s", cUS_Statistics_Info);

		FormattedExec("CREATE TABLE IF NOT EXISTS [%s] ([ResetTime] INTEGER);", cUS_Statistics_Info);

		// insert creation timestamp
		time_t now = time(nullptr);
		FormattedExec(
			"INSERT INTO [%s] ([ResetTime]) VALUES(" I64FMTD ");",
			cUS_Statistics_Info, now);
	}

	// convert data from 1.01 to 1.1
	// part #1 - prearrangement
	bool finalizeConversion = false;
	int32 version = GetUserVersion();
	if (version == 0 && TableExists(cUS_Scores))
	{
		// rename old tables - to be able to insert new table structures
		FormattedExec("ALTER TABLE %s RENAME TO us_scores_101;", cUS_Scores);
		FormattedExec("ALTER TABLE %s RENAME TO us_songs_101;", cUS_Songs);

		finalizeConversion = true; // means: conversion has to be done!
	}

	// Set version number after creation
	if (version == 0)
		SetUserVersion(cDBVersion);

	// SQLite does not handle VARCHAR(n) or INT(n) as expected.
	// Texts do not have a restricted length, no matter which type is used,
	// so use the native TEXT type. INT(n) is always INTEGER.
	// In addition, SQLiteTable3 will fail if other types than the native SQLite
	// types are used (especially FieldAsInteger). Also take care to write the
	// types in upper-case letters although SQLite does not care about this -
	// SQLiteTable3 is very sensitive in this regard.
	FormattedExec(
		"CREATE TABLE IF NOT EXISTS [%s] ("
		"[SongID] INTEGER NOT NULL, "
		"[Difficulty] INTEGER NOT NULL, "
		"[Player] TEXT NOT NULL, "
		"[Score] INTEGER NOT NULL, "
		"[Date] INTEGER NULL"
		");", cUS_Scores);

	FormattedExec(
		"CREATE TABLE IF NOT EXISTS [%s] ("
		"[ID] INTEGER PRIMARY KEY, "
		"[Artist] TEXT NOT NULL, "
		"[Title] TEXT NOT NULL, "
		"[TimesPlayed] INTEGER NOT NULL, "
		"[Rating] INTEGER NULL"
		");", cUS_Songs);

	// Add Date column to cUS_Scores
	if (!ColumnExists(cUS_Scores, "Date"))
	{
		sLog.Info("Database::Init", "Adding column [Date] to %s", cUS_Scores);
		FormattedExec("ALTER TABLE %s ADD COLUMN [Date] INTEGER NULL", cUS_Scores);
	}

	// Add Rating column to cUS_Songs
	// Just for users of nightly builds and developers!
	if (!ColumnExists(cUS_Songs, "Rating"))
	{
		sLog.Info("Database::Init", "Adding column [Rating] to %s", cUS_Songs);
		FormattedExec("ALTER TABLE %s ADD COLUMN [Rating] INTEGER NULL", cUS_Songs);
	}

	// convert data from previous versions
	// part #2 - accomplishment
	if (finalizeConversion)
	{
		// convert data from 1.01 to 1.1
		if (TableExists("us_scores_101"))
			ConvertFrom101To110();
	}
}

void UsdxDatabase::ConvertFrom101To110()
{
	if (!ColumnExists("us_scores_101", "Date"))
	{
		sLog.Info(
			"UsdxDatabase::Convert101To110",
			"Outdated song database found - begin conversion from V1.01 to V1.1");

		// insert old values into new tables
		FormattedExec(
			"INSERT INTO %s "
			"SELECT SongID, Difficulty, Player, Score, 'NULL' FROM us_scores_101;",
			cUS_Scores);
	}
	else
	{
		sLog.Info(
			"UsdxDatabase::Convert101To110",
			"Outdated song database found - begin conversion from V1.01 Challenge Mod to V1.1");

		// insert old values into new tables
		FormattedExec(
			"INSERT INTO %s "
			"SELECT SongID, Difficulty, Player, Score, Date FROM us_scores_101;",
			cUS_Scores);
	}

	FormattedExec(
		"INSERT INTO %s "
		"SELECT ID, Artist, Title, TimesPlayed, 'NULL' FROM us_songs_101;",
		cUS_Songs);

	/*
    // now we have to convert all the texts for unicode support:

    // player names
    TableData := nil;
    try
      TableData := ScoreDB.GetUniTable(
        'SELECT [rowid], [Player] ' +
        'FROM [' + cUS_Scores + '];');

      // Go through all Entrys
      while (not TableData.EOF) do
      begin
        // Convert name into UTF8 and alter all entrys
        DecodeStringUTF8(TableData.FieldByName['Player'], tempUTF8String, encCP1252);
        ScoreDB.ExecSQL(
          'UPDATE [' + cUS_Scores + '] ' +
          'SET [Player] = ? ' +
          'WHERE [rowid] = ? ',
          [tempUTF8String,
          TableData.FieldAsInteger(TableData.FieldIndex['rowid'])]);

        TableData.Next;
      end; // while

    except
      on E: Exception do
        Log.LogError(E.Message, 'TDataBaseSystem.Convert101To110');
    end;

    TableData.Free;

    // song artist and song title
    TableData := nil;
    try
      TableData := ScoreDB.GetUniTable(
        'SELECT [ID], [Artist], [Title] ' +
        'FROM [' + cUS_Songs + '];');

      // Go through all Entrys
      while (not TableData.EOF) do
      begin
        // Convert Artist into UTF8 and alter all entrys
        DecodeStringUTF8(TableData.FieldByName['Artist'], tempUTF8String, encCP1252);
        //Log.LogError(TableData.FieldByName['Artist']+' -> '+tempUTF8String+' (encCP1252)');
        ScoreDB.ExecSQL(
          'UPDATE [' + cUS_Songs + '] ' +
          'SET [Artist] = ? ' +
          'WHERE [ID] = ?',
          [tempUTF8String,
          TableData.FieldAsInteger(TableData.FieldIndex['ID'])]);

        // Convert Title into UTF8 and alter all entrys
        DecodeStringUTF8(TableData.FieldByName['Title'], tempUTF8String, encCP1252);
        ScoreDB.ExecSQL(
          'UPDATE [' + cUS_Songs + '] ' +
          'SET [Title] = ? ' +
          'WHERE [ID] = ? ',
          [tempUTF8String,
          TableData.FieldAsInteger(TableData.FieldIndex['ID'])]);

        TableData.Next;
      end; // while

    except
      on E: Exception do
        Log.LogError(E.Message, 'TDataBaseSystem.Convert101To110');
    end;
	*/

	// Now drop old tables
	Exec("DROP TABLE us_scores_101;");
	Exec("DROP TABLE us_songs_101;");
}
