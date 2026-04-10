# Event System
After **v.0.8** added events support
Added following methods for event system,
```angelscript
//Added for callback definition
void EventCallback(string&in name, CallbackItem@ item = null);

//The event name format is eventName#yourTag. The tag part is optional and is included only to allow events to be removed directly using the tag.
bool On(string&in name, EventCallback@ item = null);

//For unregistering event
bool Off(string&in name, EventCallback@ item = null);

//For triggering event
uint Trigger(string&in name, CallbackItem@ item = null, bool callAll = false);
```


# Event Example
Plugin 1:
```angelscript
void PluginInit()
{
	//User Connected From
	g_Module.ScriptInfo.SetAuthor( "Test" );
	g_Module.ScriptInfo.SetContactInfo( "Test" );


  g_Scheduler.SetInterval("TriggeringEvent", g_fInterval, g_Scheduler.REPEAT_INFINITE_TIMES);

}
void TriggeringEvent()
{
    //CallbackItem is optional for triggerin events, you can pass null
   	CallbackItem@ myItem  = CallbackItem();

    //You can pass  data
    @myItem.Data = any(12345);

    //You can pass dictionary
    @myItem.Dict = {};

    //To override the StopCall assignment of plugins, you must set this value to true.
    bool callAll = true;
    int tiggeredCount = SE_EVENT.Trigger("trigger_test", myItem, callAll);
    if(tiggeredCount > 0)
    {
        //do stuff
    }
    

}
```


Plugin 2
  ```angelscript
  void PluginInit()
  {
  	//User Connected From
  	g_Module.ScriptInfo.SetAuthor( "Test" );
  	g_Module.ScriptInfo.SetContactInfo( "Test" );

    //Or SE_EVENT.On("trigger_test");
    //This tag is added to allow this event to be deleted, but its optional.
  	SE_EVENT.On("trigger_test#mytag", function (name, callback) {
      //Prevent to call other events, this variable will ignored if when  stopCall
      callback.StopCall = true;
      //Do stuff after triggering
  	});
    //Event without tag
  	SE_EVENT.On("trigger_test", function (name, callback) {
      //Do stuff
  	});
  }
  void MapChange()
  {
      //This command will only delete the command.executed event that contains #mytag.
      SE_EVENT.Off("command.executed#mytag");

      //This command will delete all command.executed#mytag event,
      SE_EVENT.Off("command.executed");

      //Or you can pass Callback parameter to remove event
      //SE_EVENT.Off("command.executed", @mycallback);
  }
```
