ref HackingModConfig m_HackingModConfig;

static string HackingModConfigPATH = "$profile:\\HackingMod.json";

ref NotificationSystem HackingModNotifications = new ref NotificationSystem();

class HackingModConfig
{ 
	//Default Values
	string ConfigVersion = "1";
	ref array<ref HackableItem> HackableItems = new ref array<ref HackableItem>;
	bool ScriptLogging = true;
		
	
	// Load config file or create default file if config doesn't exsit
	void Load(){
		if(GetGame().IsServer()){
			if (FileExist(HackingModConfigPATH)){ //If config exist load File
				JsonFileLoader<HackingModConfig>.JsonLoadFile(HackingModConfigPATH, this);
			}else{ //File does not exist create file
				HackableItems.Insert( new ref HackableItem("tent", 45, 600, 1));
				HackableItems.Insert( new ref HackableItem("fence", 60, 600, 2));
				HackableItems.Insert( new ref HackableItem("bbp_t1", 60, 900, 2));
				HackableItems.Insert( new ref HackableItem("bbp_t2", 90, 1200, 3));
				HackableItems.Insert( new ref HackableItem("bbp_t3", 120, 1800, 5));
				Save();
			}
		}
	}
	
	void Save(){
		JsonFileLoader<HackingModConfig>.JsonSaveFile(HackingModConfigPATH, this);
	}
	
	bool CanHack(string ItemType, int Batteries){
		HackableItem target = GetHackableItem( ItemType );
		if ( target.Type != ""){
			if (Batteries >= target.Batteries){
				return true;
			}
		}
		return false;
	}
	
	float GetStartTime(string ItemType){
		return GetHackableItem(ItemType).StartTime;
	}
	
	
	float GetHackTime(string ItemType){
		return GetHackableItem(ItemType).HackingTime;
	}
	
	HackableItem GetHackableItem( string ItemType){
		string itemType = ItemType;
		itemType.ToLower();		
		for (int i = 0; i < HackableItems.Count(); i++){
			string toFind = HackableItems.Get(i).Type;
			toFind.ToLower();
			if (itemType.Contains(toFind)){
				return HackableItems.Get(i);
			}
		}
		return new HackableItem();
	}
	
	bool NewSetup(){
		return false;
	}
	
	void SetupComplete(){
		//Save();
	}
};


class HackableItem {
	
	string Type = "";
	float StartTime = 90;
	float HackingTime = 600;
	int Batteries = 3;
	float TabletDamage = 15;
	float ChanceOfInterrupt = 0.01;
	
	void HackableItem( string type = "", float start = 60, float time = 600, int bat = 1)
	{
		Type = type;
		StartTime = start;
		HackingTime = time;
		Batteries = bat;
	}

};

//Helper function to return Config
static ref HackingModConfig GetHackingModConfig()
{
	if (!m_HackingModConfig)
	{
		m_HackingModConfig = new HackingModConfig;
		m_HackingModConfig.Load();
	}
	return m_HackingModConfig;
};
