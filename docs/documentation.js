// Documentation Data

/*
 **Supported Formatting in Body:**

- **Paragraphs**: Separate with double newlines (`\n\n`)
- **Headings**: Use `### Heading Text`
- **Bold**: `**bold text**`
- **Code**: `` `inline code` ``
- **Code Blocks**: 
```
```cpp
  code here
```
**Images**: ![Alt Text](path/tp/image.png)
** Links**: [Link text] (url)
** Lists **: Start lines with - or *, 1. for numbered
** Note Boxes: > **Note:** Your message - use \n\n to end
** Warnings: > **Warning:** Your Message use \n\n to end
*/

const DOCUMENTATION_DATA = {
  "title": "RPG Dialog System Documentation",
  "sections": [
    {
      "id": "introduction",
      "title": "Introduction",
      "body": `Welcome to the DMTesseracT Dialog System documentation.\n
          This guide is designed to help you get up and running quickly, understand the core workflow, and make the most of the tools we’ve built for branching, reactive narrative design in Unreal Engine.\n
          Whether you're integrating the system for the first time or exploring more advanced features, this documentation will walk you through every step with clear explanations and practical examples.`    },
    {
      "id": "before-you-begin",
      "title": "Before You Begin",
      "body": `The RPG Dialog System, by DMTesseracT Ltd, is built with a strong separation of responsibilities:\n
the plugin handles the dialog logic, flow control, state management, and sequencing, while your project remains in charge of how the game actually presents those moments to the player.
\n\n
Before you begin using the system, it’s helpful to understand a few key principles:\n
### 1. Presentation Is Project-Specific by Design

The plugin does not enforce a specific camera system, animation framework, or UI layout.
Instead, it provides the signals and data you need to drive your own:\n\n

- Cameras\n\n
- Character animations\n
- UI widgets\n
- Player interaction logic\n

This gives you complete freedom to match the style, tone, and technical requirements of your project.\n\n

### 2. Events Drive Your Gameplay Logic\n\n

The system uses lightweight blueprint interfaces to communicate with your project:\n\n

- Dialog Interface\n

- Dialog Inventory Interface\n

- Dialog Quest Interface\n

These allow the dialog system to trigger gameplay events - such as giving items, updating quests, or branching based on player state - without you ever modifying the plugin code.\n\n
Your project provides the logic; the plugin simply calls into it.\n\n

### 3. Sequencer Integration Is Required\n\n

Dialog playback is driven by Unreal’s Level Sequence system.\n
This means:\n\n

- Cinematic animation, audio, and timing are authored in Sequencer\n

- Subtitles sync via simple named markers in the sequence\n

- Any character animation or camera movement is fully under your control\n\n

If your project already uses Sequencer, the workflow will feel instantly familiar.\n\n

`  },

    {
      "id": "quick-start",
      "title": "Quick Start",
      "body": `To get a minimum working setup. Follow these instructions. \n\n### 1. Create a new Dialog Tree \n\nTo create a new Dialog Tree asset, right click in the content drawer, navigate to Rpg Dialog > Dialog Tree. \n\n![Create DT](images/create-DT.png)\n\n
   ### 2. Create a dialog node \n\n Right click in the graph and Add a Dialog Node \n\n![Create Dialog Node](images/quick-start-new-node.png)\n\n
   ### 3. Ensure your dialog node is flagged as 'Entry Point' and that you've assigned a Level Sequence to Play. \n\n![Minimal Dialog Setup](images/dialog-node-minimal.png)\n\n
  > **Warning:** You must assign a level sequence to a dialog node in order for it to play. The timing of a single dialog node is dictated by the length of the sequence provided. If no sequence is provided, the node will forcefully abort the entire Dialog Tree, to avoid a soft lock.\n\n
   ### 4. Optionally add conditions and event subnodes to your Dialog Node by right clicking on the node.\n\n ![Create Subnodes](images/add-subnode.png)\n\n
   ### 5. Add Player choice nodes and connect the Player Choices output pin from the Dialog Node to the InDialog Pin on the Choice node \n\n![Add Player Choices](images/player-choices.png)\n\n
   > **Note:** You can link as many player choices as you like to a single dialog node, and link multiple dialog nodes to the same choices.\n\n
   > **Note:** You can add conditional and event subnodes to player choices as well as to dialog nodes. Giving you complete control over gameplay flow. If a choice node has a condition that isn't met, it won't be made available, even if a dialog node is connected. \n\n
   ### 6. Build out your tree as needed. \n\n![Example Tree](images/advanced-tree.png)\n\n
   ### 7. Add an NPC Dialog Component to your NPC.\n\n![Add Dialog Component](images/add-npc-dialog-component.png)\n\n
   ### 8. Assign your Dialog Tree to the NPCDialog Component\n\n ![Assign the Dialog Tree to the component](images/assign-tree.png)\n\n
   ### 9. Setup your logic flow to invoke NPCDialog->BeginInteract in your NPC's blueprint where appropriate.\n\n![Invoke BeginInteract From Blueprint](images/start-dialog.png)\n\n
  
   `
    },

    {
      "id": "Nodes",
      "title": "Dialog Tree Overview",
      "body": `The dialog tree editor is the main way to design intricate, branching dialog trees.\n\n
      Trees consist of just two main nodes: Dialog nodes, and Player Choice nodes.\n\n 
      
      ### Dialog nodes\n\n 
      ![Dialog Node](images/dialog-node.png)\n\n
       Dialog nodes represent the cinematic dialog that should be played. They always act as the entry point to a dialog, and can branch directly into a subsequent dialog or can have an unlimited number of choice nodes linked.\n\n
      > **Note:** Dialog nodes can only output to a single subsequent dialog node OR link one or more player choice nodes. If you connect both node types as an output, the connected choices will be skipped entirely at runtime, and the next dialog node's sequence will automatically be triggered when the current sequence finishes.\n\n
      
      ### Dialog Node Properties\n\n
      ![Dialog Properties](images/dialog-properties.png)\n\n
      
      **Level Sequence To Play**\n\n
      REQUIRED. This specifies the level sequence that represents this dialog beat. A dialog node must have a valid level sequence, as the flow control is dicated by the end of the sequence. 
      If a dialog node with no level sequence is visited, the entire tree is aborted. \n\n
      
      **Entry Point**\n\n
      If specified, this dialog node is nominated as a potential start point for a dialog tree. When "Begin Interact" is called, a dialog tree will be traversed for all nodes that have Entry Point flagged. \n
      You can use a variety of conditions and priority values to control where an interaction begins.\n\n
      
      **Start Priority**\n\n
      If a dialog node is flagged as an entry point, you can use Start Priority to help guide the selection process, along with conditions. If two or more nodes qualify as an entry point
       the highest start priority will dictate which one is entered. If two qualified nodes share the same priority, which path is selected cannot be guaranteed.\n\n
      
       **Subtitles**\n\n
      An array of FText that represents the dialog being spoken.\n\n
      > **Note:** See Setting Up Subtitles section for detailed information on informing UMG of the currently relevant subtitle during a sequence. \n\n
      
      **Skippable**\n\n
      This is the main way to control a players ability to skip a scene. If set to true, this dialog can be skipped during playback by calling "Skip Sequence" via the Dialog Manager Subsystem. if false, nothing will be able to interrupt the sequence. 
      
      
     \n\n### Player Choice Nodes\n\n
      \n\n![Player Choice Node](images/choice-node.png)\n\n
      Player choice nodes act as a fork in a dialog tree.  When assigned to a dialog node, they'll be conditionally applicable at the end of that dialog's sequence. \n
      When a dialog node finishes, if it has any qualified player choices (See Conditional Subnodes), the dialog will pause until a choice node is selected.
      
      \n\n
      ### Player Choice Properties\n\n

      **Choice Text**\n\n
      The UI text that should be displayed for this choice.\n\n


      ### Conditional Subnodes\n\n
      Conditional subnodes can be added to Dialog and Player Choice nodes by right clicking a node and selecting an appropriate condition. There are 3 
      conditional nodes: State, Quest and Inventory. Parent nodes can have any number and combination of conditional nodes as required.  \n\n

      > **Warning:** Conditional nodes added to Dialog nodes are explicitly ignored if the Dialog node is invoked anywhere outside of the Entry Node of a dialog tree.
      This is by design, since all mid-tree dialog nodes are expected to have been accessed by a pre-validated branch. e.g. if a dialog is triggered by a Player Choice,
      then it should always be triggered. In instances where you want a dialog node to be the entry point as well as a loop within a tree, you can safely add Conditional nodes
      to it, which will only be validated when selecting the entry point.\n\n

      ### Event Subnodes\n\n
      Event subnodes can be added to Dialog and player Choice nodes in the same way as Conditional Subnodes, by right clicking the target node and selecting the appropriate event from
      the context menu.\n 
      There are 4 types of event nodes: State, Quest, Inventory and Custom Script Events.\n\n

      **State Event**\n\n
      State events can set the value of an interntal Dialog State key/value pair when the parent node is traversed. \n\n

      **Quest Event**\n\n
      A Quest event can dispatch Quest events to the Player Pawn via the Dialog Quest Interface.\n\n
      
      **Inventory Event**\n\n
      An Inventory event can dispatch Inventory events to the Player Pawn via the Dialog Inventory Interface.\n\n

      **Custom Script Event**\n\n
      Custom Script events trigger custom scripts that subclass the DialogEventScriptBase class. Allowing for more bespoke control over events.\n\n

      `
    },
    {
      "id": "dialogInterfaces",
      "title": "Dialog Interfaces",
      "body":`
        ### Dialog Interface\n\n
        Dialog interface includes core functions surrounding events in a dialog tree. Both the player and any NPC that has an NPC Dialog component should implement
        this interface.  These events are used to control user input throughout a dialog interaction.\n\n 
        
        **Event: BeginDialogTreeInteraction()**\n
        This event is invoked when BeginInteract() successfully enters a dialog tree. This is implicitly invoked on the player pawn and the NPC that owns the Dialog Tree.\n
        This is a common place to setup static camera changes and user input states for the interaction. \n\n
        ![Begin Dialog Tree Event](images/DI_BeginTree.png)\n\n

        **Event: EndDialogTreeInteraction()**\n
        This event is invoked when an active Dialog Tree reaches the end of a branch. This is implicitly invoked on the player pawn and the NPC that owns the Dialog Tree.\n
        Often used as a place to restore the gameplay view and control to the player.\n\n
        ![End Dialog Tree Event](images/DI_EndTree.png)\n\n

        **Event: DialogSequenceInitialised(FNpcDialogSequence Sequence)**\n
        This event is invoked whenever a new Dialog Node is reached within a Dialog Tree. Can be used to act on any general information from the current sequence. Such as getting all subtitles
        for the sequence that's commencing.\n\n\n
        ![Sequence Initialised Event](images/DI_SequenceStart.png)\n\n

        **Event: DialogSequenceEnded(FNpcDialogSequence Sequence)**\n
        Invoked when a dialog node is exiting. This is generally where you can control setting up UI elements for the player choice selection.\n\n\n
        ![Sequence Ended Event](images/DI_SequenceEnded.png)\n\n

        **Event: OnFailedToPlayDialogTree(const FString& Reason)**\n
        Invoked when a Dialog Tree BeginInteract() attempt has failed - Usually due to there being no valid Entry Point in the target Dialog Tree (conditional fails etc.)\n
        Useful for knowing when an NPC isn't interested in talking.\n\n
        ![Dialog Failed Event](images/DI_DialogFailed.png)\n\n


        ### Dialog Quest Interface\n\n
        Quests interface is used by the subnodes to dispatch appropriately. This is where you should hook in your existing quest system to respond to dialog events and queries. 
        Care has been given to try to cover all possible quest states. You only need to implement the functions that suit your system, and use. \n\n

        	 
	**bool QuestDialogSelected(FName EventId)**\n\n
  Notify that a quest dialog option was selected. This event is triggered by Quest Event subnode type: PROGRESS_QUEST.\n\n
	![Quest Dialog Selected](images/DQI_DialogSelected.png)\n\n

  
  **bool AddQuest(const FName QuestId)**\n\n
  Add a quest to the player's quest log by ID.Called by Quest Event subnode type: ADD_QUEST\n\n
	![Add Quest](images/DQI_AddQuest.png)\n\n

	
  **bool RemoveQuest(const FName QuestId)**\n\n
  Remove a quest from the player's quest log by ID. Called by Quest Event subnode type: REMOVE_QUEST\n\n
  ![Remove Quest](images/DQI_RemoveQuest.png)\n\n


	**bool CompleteQuest(const FName QuestId)**\n\n
  Complete a quest by ID. Called by Quest Event subnode type: COMPLETE_QUEST \n\n
  ![Complete Quest](images/DQI_CompleteQuest.png)\n\n

	
  **bool FailQuest(const FName QuestId)**\n\n
  Fail a quest with the specific id. Called by Quest Event subnode type: FAIL_QUEST \n\n
  ![Fail Quest](images/DQI_FailQuest.png)\n\n


	**bool HasQuest(const FName QuestId)**\n\n
  Check if the player has a specific quest by ID.Called by Quest Conditional subnode type: HAVE \n\n
  ![Has Quest](images/DQI_HasQuest.png)\n\n

	**bool CompletedQuest(const FName QuestId)**\n\n
	Check if the player has completed a specific quest by ID. This should return true if the quest is complete and not actively tracked (e.g. has been "claimed" or "handed in")
  Called by Quest Conditional subnode type: HAS_COMPLETED\n\n
  ![Completed Quest](images/DQI_CompletedQuest.png)\n\n

	**bool IsQuestReadyForTurnIn(const FName QuestId)**\n\n
  Check if the player has an active quest by ID, and that all objectives are complete. This differs from CompletedQuest, in that the quest should be ready to "claim" but has not yet been. 
  Called by Quest Conditional subnode type: IS_COMPLETE\n\n
  ![Is Quest Ready For Turn-In](images/DQI_IsQuestReady.png)\n\n

	**bool HasFailedQuest(const FName QuestId)**\n\n
  Check if a quest has been failed previously. Called by Quest Conditional subnode type : HAS_FAILED\n\n
  ![Has Failed Quest](images/DQI_HasFailed.png)\n\n

  ### Dialog Inventory Interface\n\n

  **bool HasItem(const FName ItemId)**\n\n
  
  **bool HasNumItem(const FName ItemId, const int Num)**\n\n

  **bool AddItem(const FName ItemId, const int NumItems)**\n\n

  **bool RemoveItem(const FName ItemId, const int NumItems)**\n\n

      `
    },
    {
      "id":"subsystem",
      "title":"Dialog Manager Subsystem",
      "body":`
      The manager subsystem is a World Subsystem that globally communicates dialog events. While most core dialog events are automatically sent to the relevant actors
      (the player and the NPC that owns the tree), there are circumstances where other UObjects may want to be included in the events. For every Dialog Interface event,
      there is a delegate in the subsystem - along with some other unique delegates - that can be bound by any class that need them. This is also where player choices are 
      communicated to the current sequence, and subtitle marker events are broadcast.\n\n
      ![Dialog Manager Subsystem](images/DialogManagerSubsystem.png)\n\n

      **OnSubtitleMarkerHit Delegate**\n\n
      ![OnSubtitleMarkerHit Delegate](images/Subsystem_OnSubtitleMarkerHit.png)\n\n

      **OnDialogStarted Delegate**\n\n
      ![OnDialogStarted Delegate](images/Subsystem_OnDialogStarted.png)\n\n

      **OnDialogEnded Delegate**\n\n
      ![OnDialogEnded Delegate](images/Subsystem_OnDialogEnded.png)\n\n

      **OnNewSequence Delegate**\n\n
      ![OnNewSesquence Delegate](images/Subsystem_OnNewSequence.png)\n\n 

      **OnSequenceFinished Delegate**\n\n
      ![OnSequenceFinished Delegate](images/Subsystem_OnSequencecEnded.png)\n\n

      **OnDialogFailedToStart Delegate**\n\n
      ![OnDialogFailedToStart](images/Subsystem_OnDialogFailed.png)\n\n

      **GetCurrentSequenceQualifiedChoices**\n\n
      Returns a filtered list of player choices that are linked to the sequence that's currently playing. Only choices that meet all of their conditions are returned. 
      This is the primary way to build UMG widgets out of the choices that should be displayed to a player at the end of a sequence.\n\n
      ![GetCurrentSequenceQualifiedChoices node](images/Subsystem_GetQualifiedChoices.png)\n\n

      **SelectDialogChoice**\n\n
      ![SelectDialogChoice Node](images/Subsystem_SelectDialogChoice.png)\n\n

      **SkipSequence**\n\n
      ![SkipSequence Node](images/Subsystem_SkipSequence.png)\n\n

      `
    },
    {
      "id": "subtitles",
      "title": "Subtitles",
      "body": `
        ### Level Sequence Markers\n\n
        ### Binding UI to subtitle cues\n\n        
      `
    },
    {
      "id":"advancedInfo",
      "title":"Common Implementations and examples",
      "body":`
      ### Setting Up Dialog Gameplay\n\n
      **Handle dialog tree start**\n\n
      **Handling and selecting player choice events**\n\n
      **Handle dialog tree end**\n\n
      `
    }
  ]
};