ref HackingModConfig m_HackingModConfig;

ref NotificationSystem HackingModNotifications = new ref NotificationSystem();

//Helper function to return Config
static ref HackingModConfig GetHackingModConfig()
{
	if (!m_HackingModConfig)
	{
		Print("[HackingMod] Init");
		m_HackingModConfig = new HackingModConfig;
		m_HackingModConfig.Load();
	}
	return m_HackingModConfig;
};
