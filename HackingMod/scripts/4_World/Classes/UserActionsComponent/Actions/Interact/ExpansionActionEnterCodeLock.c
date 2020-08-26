modded class ExpansionActionEnterCodeLock: ActionInteractBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		TentBase tent = TentBase.Cast( target.GetParent() );

		if ( tent  && GetHackingModConfig().AllowCodeLocksOnTents )
		{
			m_Target = ItemBase.Cast( target.GetParent() );
			if ( m_Target )
			{
				string selection = m_Target.GetActionComponentName( target.GetComponentIndex() );
				if (m_Target.ExpansionHasCodeLock( selection ) && !m_Target.IsOpened()){
					return true;
				} else if (m_Target.IsLocked() || (m_Target.HasCode() && !m_Target.IsOpened())){
					return true;
				}
			}		
			return false;
		}
		
		return super.ActionCondition( player, target, item );
	}
};
