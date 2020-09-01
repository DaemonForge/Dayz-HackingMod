class HackingModNotificationWidget
{	
	protected ref Widget				m_Root;
	protected ref Widget				m_Notification;
	protected ref ImageWidget			m_Icon;
	protected ref TextWidget			m_Heading;
	protected ref MultilineTextWidget	m_Message;
	
	protected float 			m_notificationX;
	protected float 			m_notificationY;
	protected float 			m_notificationAlpha;
	protected float 			m_notification0Timer;
	protected float 			m_notificationTimer;
	
	void HackingModNotificationWidget()
    {
		m_Root 			= GetGame().GetWorkspace().CreateWidgets("HackingMod/GUI/Layouts/HackingNotification.layout");
		m_Notification	= Widget.Cast( m_Root.FindAnyWidget( "Notification" ) );
		m_Icon	        = ImageWidget.Cast( m_Root.FindAnyWidget( "Icon" ) );
		m_Heading	    = TextWidget.Cast( m_Root.FindAnyWidget( "Heading" ) );
		m_Message	    = MultilineTextWidget.Cast( m_Root.FindAnyWidget( "Message" ) );
    }

	void ~HackingModNotificationWidget()
    {
		m_Notification.SetAlpha(0.001);
		m_Icon.SetAlpha(0.001);
		m_Heading.SetAlpha(0.001);
		m_Message.SetAlpha(0.001);
		m_Notification.Show(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessIn);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessOut);
    }
	

	void Init(string Heading, string Message, string NotificationIcon = "HackingMod/GUI/Images/hacking.paa", float timeSeconds = 8)	
	{
		Print("Notification Init");
		m_Heading.SetText(Heading);
		m_Message.SetText(Message);
		m_Icon.LoadImageFile(0, NotificationIcon, true);
		m_notificationAlpha = 0.001;
		m_Notification.SetAlpha(m_notificationAlpha);
		m_Icon.SetAlpha(m_notificationAlpha);
		m_Heading.SetAlpha(m_notificationAlpha);
		m_Message.SetAlpha(m_notificationAlpha);
		m_notificationX = -550;
		m_notificationY = 256;
		m_Notification.SetPos(m_notificationX, m_notificationY, true);
		m_notification0Timer = 1;
		m_notificationTimer = timeSeconds;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ProccessIn, 13, false);
	}
	
	void ProccessIn()	
	{
		if (m_notificationAlpha <= 0.988){
			m_notificationAlpha = m_notificationAlpha + 0.012;
		} else {
			m_notificationAlpha = 1;
		} 
		
		m_Notification.SetAlpha(m_notificationAlpha * 0.90);
		
		m_Icon.SetAlpha(m_notificationAlpha * 0.95);
		m_Heading.SetAlpha(m_notificationAlpha * 0.95);
		m_Message.SetAlpha(m_notificationAlpha * 0.95);
		
		if ( m_notificationX >= 8 && m_notificationAlpha == 1 && m_notification0Timer > 0){
			m_notification0Timer--;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ProccessOut, m_notificationTimer * 1000, false);
		} else if (m_notification0Timer != 0){
			if (m_notificationX < 8) {
				m_notificationX = m_notificationX + 7;
				if (m_notificationX > 8){
					m_notificationX = 8;
				}
				m_Notification.SetPos(m_notificationX, m_notificationY, true);
			}
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ProccessIn, 12, false);
		}
	}
	
	void ProccessOut() {
		
		if (m_notificationAlpha >= 0.029){
			m_notificationAlpha = m_notificationAlpha - 0.028;
		} else {
			m_notificationAlpha = 0;
		} 
		
		if (m_notificationAlpha != 0){
			m_Notification.SetAlpha(m_notificationAlpha * 0.90);
			
			m_Icon.SetAlpha(m_notificationAlpha * 0.95);
			m_Heading.SetAlpha(m_notificationAlpha * 0.95);
			m_Message.SetAlpha(m_notificationAlpha * 0.95);
			
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ProccessOut, 12, false);
		} else {
			m_Notification.Show(false);
			m_Icon.Show(false);
			m_Heading.Show(false);
			m_Message.Show(false);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.Delete, 50, false);
		}
	}
	
	void EarlyProccess()
	{
		m_notification0Timer = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessIn);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessOut);
		ProccessOut();
	}
	
	void Delete(){
		m_Notification.Show(false);
		m_Icon.Show(false);
		m_Heading.Show(false);
		m_Message.Show(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessIn);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ProccessOut);
	}
}