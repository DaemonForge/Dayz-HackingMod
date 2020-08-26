modded class ActionNextCombinationLockDialOnTarget: ActionInteractBase
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if (super.ActionCondition( player, target, item )){
			Fence building = Fence.Cast( target.GetObject() );
			if ( building )
			{
				if ( building.ExpansionHasCodeLock( "codelock" ) )
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
};
