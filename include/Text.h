#define BRAWL_ORIGINAL_ITEM_NAME  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Beast Eye</FONT></TEXTFORMAT>"
#define BRAWL_NEW_ITEM_NAME  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Melee Mayhem</FONT></TEXTFORMAT>"
#define BRAWL_ORIGINAL_ITEM_DESCRIPTION  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Said to tremble when close to Deathroot</FONT></TEXTFORMAT>"
#define BRAWL_NEW_ITEM_DESCRIPTION  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Start a Brawl</FONT></TEXTFORMAT>"
#define BRAWL_ORIGINAL_DIALOG_TEXT "<TEXTFORMAT LEADING='4'><FONT LETTERSPACING='0'>Return to your world?</FONT></TEXTFORMAT>"
#define BRAWL_NEW_DIALOG_TEXT "<TEXTFORMAT LEADING='4'><FONT LETTERSPACING='0'>Do you want to start a Brawl?</FONT></TEXTFORMAT>"

#define TEAMFIGHT_ORIGINAL_ITEM_NAME  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Memory of Grace</FONT></TEXTFORMAT>"
#define TEAMFIGHT_NEW_ITEM_NAME  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Team Turmoil</FONT></TEXTFORMAT>"
#define TEAMFIGHT_ORIGINAL_ITEM_DESCRIPTION  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Lose all runes, return to last site of lost grace visited</FONT></TEXTFORMAT>"
#define TEAMFIGHT_NEW_ITEM_DESCRIPTION  "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Start a Team Fight</FONT></TEXTFORMAT>"
#define TEAMFIGHT_ORIGINAL_ITEM_LONG_DESCRIPTION "<TEXTFORMAT LEADING='-6'><FONT LETTERSPACING='0'>Ashen remains in which spirits yet dwell.\nUse to summon a spirit jellyfish.\n\nA floating spirit that illuminates its surroundings.\nProne to tears, the jellyfish girl searches for her distant home. Will bravely spew poison at foes on her summoner's behalf.\n\nIt seems her name is Aurelia.</FONT></TEXTFORMAT>"
#define TEAMFIGHT_NEW_ITEM_LONG_DESCRIPTION "<TEXTFORMAT LEADING='-6'><FONT LETTERSPACING='0'>Line 1.\nLine 2.\n\nLine 3.\nLine4.</FONT></TEXTFORMAT>"
#define TEAMFIGHT_ORIGINAL_DIALOG_TEXT "<TEXTFORMAT LEADING='4'><FONT LETTERSPACING='0'>Lose all runes to return to the last site of grace visited?</FONT></TEXTFORMAT>"
#define TEAMFIGHT_NEW_DIALOG_TEXT "<TEXTFORMAT LEADING='4'><FONT LETTERSPACING='0'>Do you want to start a Team Fight?</FONT></TEXTFORMAT>"

#define TARNISHED_WIZENED_FINGER_ORIGINAL_ITEM_NAME "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Scriptstone</FONT></TEXTFORMAT>"
#define TARNISHED_WIZENED_FINGER_NEW_ITEM_NAME "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Phantom Bloody Finger</FONT></TEXTFORMAT>"
#define TARNISHED_WIZENED_FINGER_ORIGINAL_ITEM_DESCRIPTION "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Reveals more messages from other worlds</FONT></TEXTFORMAT>"
#define TARNISHED_WIZENED_FINGER_NEW_ITEM_DESCRIPTION "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Activate the Phantom Bloody Finger, quickly choose a spawn point, but beware, failing to decide in time will cost your life as your health wanes.</FONT></TEXTFORMAT>"

#define SOAP_ORIGINAL_ITEM_NAME "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Soap</FONT></TEXTFORMAT>"
#define SOAP_NEW_ITEM_NAME "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Soap (Cures Rot)</FONT></TEXTFORMAT>"
#define SOAP_ORIGINAL_ITEM_DESCRIPTION "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>Removes filth and accumulations on the body</FONT></TEXTFORMAT>"
#define SOAP_NEW_ITEM_DESCRIPTION "<TEXTFORMAT LEADING='0'><FONT LETTERSPACING='0'>A rub of soap a day keeps seamless rot away.</FONT></TEXTFORMAT>"

#define YOU_DIED_ORIGINAL "<TEXTFORMAT LEADING='2'><FONT LETTERSPACING='0'>SLAIN</FONT></TEXTFORMAT>"
#define YOU_DIED "<TEXTFORMAT LEADING='2'><FONT LETTERSPACING='0'>YOU DIED</FONT></TEXTFORMAT>"


class Text {
public:
	static void ReplaceMenuText(char* original, std::string newText) {
		std::string originalText = std::string(original);
		auto newTextArray = newText.c_str();
		auto newTextArrayLength = newText.length();
		auto originalTextLength = originalText.length();
		for (int i = 0; i < originalTextLength; i++) {
			if (i < newTextArrayLength) original[i] = newTextArray[i];
			else original[i] = ' ';
		}
	}
};