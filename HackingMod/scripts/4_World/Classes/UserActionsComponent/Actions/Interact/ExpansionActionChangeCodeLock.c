modded class ExpansionActionChangeCodeLock: ActionInteractBase
{
	// -----------------------------------------------------------
	// Override ActionCondition
	// -----------------------------------------------------------
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		TentBase tent = TentBase.Cast( target.GetParent() );

		if ( tent && GetHackingModConfig().AllowCodeLocksOnTents)
		{
			m_Target = ItemBase.Cast( target.GetParent() );
			if ( m_Target )
			{
				if (m_Target.HasCode() && !m_Target.IsOpened() && !m_Target.IsLocked())
				{
					return true;
				}
			}		
			return false;
		}
		
		return super.ActionCondition( player, target, item );
	}
};
