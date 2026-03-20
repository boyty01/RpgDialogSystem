/**
 * RpgDialogSystem Plugin Documentation Data
 * Loaded by index.html before renderer.js.
 *
 * To document a new plugin, copy this file, replace all content inside
 * window.PLUGIN_DATA = { ... }, and update the <script> src in index.html.
 *
 * ── SCHEMA REFERENCE ──────────────────────────────────────────────────────
 *
 * plugin        { name, module, description, logoText?, namePrefix?, nameSuffix? }
 * overview      { title, subtitle, body?, callouts?: [{type:"note|tip|warn", text}] }
 * architecture  { body?, layers: [{num, name, items:[]}] }
 * workflows     [{ title, steps:[] }]
 * groups        [{ id, label, classes:["ClassName",...] }]
 *   — defines sidebar sections and the order classes appear in each section
 * classes       [{ id, name, parents:[], description, navDot, badges:[{text,cls}],
 *                  body:[memberGroup], callouts?:[{type,text}] }]
 *   memberGroup types:
 *     propertyTable  { title, type:"propertyTable",  rows:[{name,type,default?,desc,meta?}] }
 *     functionTable  { title, type:"functionTable",  rows:[{sig,desc,meta?}] }
 *     enumBlock      { title, type:"enumBlock",      enums:[{name,type?,values:[{name,desc}]}] }
 *     structBlock    { title, type:"structBlock",    structs:[{name,fields:[{name,desc}]}] }
 *     delegateList   { title, type:"delegateList",   delegates:[{sig,desc}] }
 *     calloutGroup   { title, type:"calloutGroup",   callouts:[{type,text}] }
 *     paragraphs     { title, type:"paragraphs",     paragraphs:[] }
 *   meta tags (on rows): [{text:"BlueprintCallable", cls:"meta-bp"}, ...]
 *     cls options: meta-bp, meta-pure, meta-native, meta-impl, meta-prot, meta-cat
 * delegatesRef  [{ sig, desc }]
 *
 * In desc/text strings: `code` renders as <code>code</code>
 *                        **text** renders as <strong>text</strong>
 */

window.PLUGIN_DATA = {

    // ── PLUGIN METADATA ───────────────────────────────────────────────────────
    plugin: {
        name:       "RpgDialogSystem",
        module:     "RpgDialogSystem",
        description:"Unreal Engine 5 — C++ Plugin",
        logoText:   "DLG",
        namePrefix: "Rpg",
        nameSuffix: "DialogSystem"
    },

    // ── OVERVIEW ──────────────────────────────────────────────────────────────
    overview: {
        title:    "RpgDialogSystem Plugin",
        subtitle: "Unreal Engine 5 — C++ Plugin Reference | Module: `RpgDialogSystem`",
        body:     "RpgDialogSystem is a data-driven NPC conversation plugin for Unreal Engine 5. It provides branching dialog trees authored in a visual graph editor, condition-based sequence qualification against quest, inventory, reputation, and state systems, cinematic level-sequence playback with frame-accurate subtitle markers, and a flexible script-event pipeline for triggering arbitrary game logic during or after dialog. All runtime types are fully exposed to Blueprint.",
        callouts: [
            {
                type: "tip",
                text: "The recommended authoring workflow is: create a `UDialogTree` data asset, open it in the **Dialog Graph Editor**, wire sequences and choices visually, then assign the asset to a `UNpcDialogComponent` on your NPC actor."
            },
            {
                type: "note",
                text: "Actors that participate in condition evaluation must implement the relevant interfaces — `IDialogQuestInterface`, `IDialogInventoryInterface`, `IDialogReputationInterface`, or `IDialogInterface` — so the system can query and mutate game state without coupling to specific pawn classes."
            },
            {
                type: "warn",
                text: "Custom script events (`UDialogScriptEventBase` subclasses) **must** call `FinishExecute()` when their logic is complete. Omitting this call will leave the subsystem tracking a zombie instance and may block subsequent dialog from starting."
            }
        ]
    },

    // ── ARCHITECTURE ──────────────────────────────────────────────────────────
    architecture: {
        body: "The plugin is organised into six conceptual layers. Higher layers depend on lower ones; lower layers have no upward dependencies.",
        layers: [
            { num: 1, name: "Global Management",    items: ["UDialogManagerSubsystem"] },
            { num: 2, name: "NPC Component",        items: ["UNpcDialogComponent"] },
            { num: 3, name: "Data Assets",          items: ["UDialogTree"] },
            { num: 4, name: "Core Types",           items: ["FNpcDialogSequence", "FDialogChoice", "FDialogCondition", "FDialogEventData", "FStandaloneDialogSequence"] },
            { num: 5, name: "Script Events",        items: ["UDialogScriptEventBase"] },
            { num: 6, name: "Interfaces",           items: ["IDialogInterface", "IDialogQuestInterface", "IDialogInventoryInterface", "IDialogReputationInterface"] }
        ]
    },

    // ── WORKFLOWS ─────────────────────────────────────────────────────────────
    workflows: [
        {
            title: "Starting a Dialog Interaction",
            steps: [
                "Player actor triggers interaction (overlap, input, etc.) and calls `UNpcDialogComponent::BeginInteract()`.",
                "Component calls `CanStartADialog()` — returns false if the `DialogTree` is null or no sequences are qualified.",
                "All sequences flagged `bConsideredForDialogStart` are evaluated by iterating their `QualifyingConditions` via `CheckDialogCondition()`.",
                "The highest-`SelectionPriority` qualified sequence wins and is stored as `CurrentDialogSequence`.",
                "`UDialogManagerSubsystem::RegisterNewInteraction()` is called, setting `bDialogIsActive` and caching `ActiveDialogNPC`.",
                "Both the player pawn and the NPC actor receive `IDialogInterface::BeginDialogTreeInteraction()` if they implement the interface.",
                "`PlaySequenceByGuid()` is called with the winning sequence ID; the level sequence begins playback."
            ]
        },
        {
            title: "Sequence Playback & Subtitles",
            steps: [
                "`UNpcDialogComponent::PlaySequence()` instantiates a `ULevelSequencePlayer` for the sequence's `LevelSequence` asset.",
                "`BindSubtitleMarkerEvents()` scans the level sequence for frame markers named `Sub_0`, `Sub_1`, etc., and caches them in `SubtitleMarkers`.",
                "Each frame `HandleSequenceUpdated()` is called; when the playhead crosses a cached marker frame, `OnSubtitleMarkerHit()` fires.",
                "`UDialogManagerSubsystem::DialogSubtitleMarkerHit()` broadcasts `OnSubtitleMarkerHit` with the subtitle index and speaker label.",
                "UI listens to the delegate and displays the matching entry from the sequence's `Subtitles` array.",
                "When the level sequence ends, `OnSequenceFinished()` fires on the component, which calls `SequenceFinished()` on the subsystem."
            ]
        },
        {
            title: "Player Choice & Sequence Chaining",
            steps: [
                "After `OnSequenceFinished`, UI queries `UDialogManagerSubsystem::GetCurrentSequenceQualifiedChoices()` to obtain the valid choice list.",
                "Each `FDialogChoice`'s `QualifyingConditions` are checked against the player via `CheckDialogCondition()`; failing choices are filtered out.",
                "Player selects a choice; UI calls `UDialogManagerSubsystem::SelectDialogChoice(Choice)`.",
                "The subsystem processes `Choice.TriggeredEvents` via `HandleEventData()`, dispatching quest, inventory, reputation, state, or script events as appropriate.",
                "If the choice has a `NextSequenceId`, `MakeDialogOptionSelection()` forwards to `UNpcDialogComponent::PlaySequenceByGuid()` to play the next sequence.",
                "If `FNpcDialogSequence::AutoSelectNextSequence` is valid, the next sequence plays automatically without presenting choices.",
                "If no next sequence exists, `EndCurrentDialog()` is called, resetting `bDialogIsActive` and broadcasting `OnDialogEndedDelegate`."
            ]
        },
        {
            title: "Dialog Event Execution",
            steps: [
                "`HandleEventData()` iterates an array of `FDialogEventData` structs from a sequence or choice.",
                "Each event's `TargetSystem` routes to the matching handler: `HandleQuestEventData`, `HandleInventoryEventData`, `HandleStateEventData`, `HandleReputationEventData`, or `HandleCustomEventData`.",
                "Quest and inventory handlers call the player pawn via `IDialogQuestInterface` or `IDialogInventoryInterface`.",
                "State handlers write directly to the NPC component's `DialogStateModifiers` map via the active `UNpcDialogComponent`.",
                "Reputation handlers call the target actor via `IDialogReputationInterface::ModifyReputation()`.",
                "Custom script handlers call `SpawnScriptEvent()`, which instantiates the `UDialogScriptEventBase` subclass and calls `NativeExecute()`.",
                "If `bDeferExecuteToEndOfDialogTree` is true the script is queued in `QueuedScriptEvents` and executed only when the full dialog tree ends."
            ]
        },
        {
            title: "Custom Script Event (Blueprint)",
            steps: [
                "Create a Blueprint subclass of `UDialogScriptEventBase`.",
                "Override `ExecuteScript(Instigator, World)` and implement the desired game logic.",
                "Use `GetPlayerPawn()`, `SpawnActor()`, or any Blueprint-accessible API within the script.",
                "Call `FinishExecute()` when the logic is complete; this notifies `UDialogManagerSubsystem` to stop tracking the instance.",
                "Set `bDeferExecuteToEndOfDialogTree = true` in the class defaults if the script should only run after the full conversation ends.",
                "Assign the script class to `FDialogEventData::ScriptClass` on the relevant event node in the dialog graph."
            ]
        },
        {
            title: "Condition Evaluation",
            steps: [
                "`CheckDialogCondition(Condition)` on `UNpcDialogComponent` is the single entry point for all condition checks.",
                "The condition's `TargetSystem` determines which sub-check runs: quest, inventory, state, or reputation.",
                "Quest checks call `IDialogQuestInterface` methods (`HasQuest`, `CompletedQuest`, `IsQuestReadyForTurnIn`, `HasFailedQuest`) on the player pawn.",
                "Inventory checks call `IDialogInventoryInterface::HasItem()` or `HasNumItem()` on the player pawn.",
                "State checks read from the NPC component's local `DialogStateModifiers` map.",
                "Reputation checks call `IDialogReputationInterface::GetReputation()` on the player pawn and compare using `AT_LEAST` or `LESS_THAN`.",
                "If `FDialogCondition::bNegate` is true, the result is inverted before being returned."
            ]
        }
    ],

    // ── SIDEBAR NAV GROUPS ────────────────────────────────────────────────────
    groups: [
        { id: "subsystem-heading",   label: "Subsystem",   classes: ["UDialogManagerSubsystem"] },
        { id: "components-heading",  label: "Components",  classes: ["UNpcDialogComponent"] },
        { id: "assets-heading",      label: "Data Assets", classes: ["UDialogTree"] },
        { id: "scripts-heading",     label: "Scripts",     classes: ["UDialogScriptEventBase"] },
        { id: "interfaces-heading",  label: "Interfaces",  classes: ["IDialogInterface", "IDialogQuestInterface", "IDialogInventoryInterface", "IDialogReputationInterface"] },
        { id: "library-heading",     label: "Library",     classes: ["URpgDialogFunctionLibrary"] },
        { id: "types-heading",       label: "Types",       classes: ["DialogTypes"] }
    ],

    // ── CLASSES ───────────────────────────────────────────────────────────────
    classes: [

        // ── UDialogManagerSubsystem ────────────────────────────────────────────
        {
            id:          "UDialogManagerSubsystem",
            name:        "UDialogManagerSubsystem",
            parents:     ["UWorldSubsystem"],
            navDot:      "dot-component",
            description: "World subsystem that acts as the single source of truth for all active dialog state. Tracks the active NPC, the current sequence, the subtitle index, and any queued or running script events. All Blueprint UI should bind to its delegates rather than polling individual components. Obtain via `GetWorld()->GetSubsystem<UDialogManagerSubsystem>()`.",
            badges: [
                { text: "WorldSubsystem",  cls: "badge-component" },
                { text: "BlueprintCallable", cls: "badge-blueprintable" }
            ],
            body: [
                {
                    title: "Delegates",
                    type:  "delegateList",
                    delegates: [
                        {
                            sig:  "FOnNewSequence OnNewSequenceDelegate",
                            desc: "Broadcast when a new `FNpcDialogSequence` begins playing. Passes the full sequence struct. Bind here to update UI with the new sequence's data."
                        },
                        {
                            sig:  "FOnSequenceFinished OnSequenceFinishedDelegate",
                            desc: "Broadcast when the active sequence's level sequence finishes. Passes the completed sequence struct. Bind here to present player choices."
                        },
                        {
                            sig:  "FOnSubtitleMarkerHit OnSubtitleMarkerHit",
                            desc: "Broadcast when a subtitle frame marker is hit during sequence playback. Passes the subtitle `Index` (into the sequence's `Subtitles` array) and the `Speaker` label string."
                        },
                        {
                            sig:  "FOnSubtitleDialogPlay OnSubtitleAudioPlay",
                            desc: "Broadcast when an NPC begins playing an audio file that supports the subtitle system. Passes the `USoundBase` asset."
                        },
                        {
                            sig:  "FOnDialogStarted OnDialogStartedDelegate",
                            desc: "Broadcast when a new dialog interaction is registered and `bDialogIsActive` becomes true."
                        },
                        {
                            sig:  "FOnDialogEnded OnDialogEndedDelegate",
                            desc: "Broadcast when the active dialog interaction ends and `bDialogIsActive` is reset to false."
                        },
                        {
                            sig:  "FOnDialogFailedToStart OnDialogFailedToStart",
                            desc: "Broadcast when dialog fails to start for any reason. Passes a `Reason` string for diagnostics."
                        },
                        {
                            sig:  "FOnStandaloneSequenceTriggered OnStandaloneSequenceTriggered",
                            desc: "Broadcast when a standalone (non-tree) dialog sequence is triggered. Passes the `FStandaloneDialogSequence` data struct."
                        },
                        {
                            sig:  "FOnStandaloneSequenceEnded OnStandaloneSequenceEnded",
                            desc: "Broadcast when a previously triggered standalone sequence ends."
                        }
                    ]
                },
                {
                    title: "Dialog Flow Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "RegisterNewInteraction(AActor* DialogNPC)",
                            desc: "Begins a new dialog interaction for the given NPC actor. Sets `bDialogIsActive`, caches `ActiveDialogNPC`, and broadcasts `OnDialogStartedDelegate`. Called internally by `UNpcDialogComponent::BeginInteract()`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "SelectDialogChoice(FDialogChoice Choice)",
                            desc: "The primary way to advance dialog from a player choice. Executes all triggered events on the choice struct, then chains to the next sequence. Prefer this over `MakeDialogOptionSelection` when you have the full struct available.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "MakeDialogOptionSelection(const FGuid SelectionId)",
                            desc: "Advances dialog to the sequence identified by `SelectionId` without executing choice events. Use `SelectDialogChoice` for full functionality; use this only when you have a GUID but not the full struct.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetCurrentSequenceQualifiedChoices() : TArray<FDialogChoice>",
                            desc: "Returns the subset of the active sequence's choices whose `QualifyingConditions` all pass for the current player state. Call this after `OnSequenceFinishedDelegate` fires to populate your choice UI.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "EndCurrentDialog()",
                            desc: "Terminates the active dialog interaction, resets `bDialogIsActive`, clears `ActiveDialogNPC`, and broadcasts `OnDialogEndedDelegate`. Also notifies the player and NPC via `IDialogInterface::EndDialogTreeInteraction()`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "SkipSequence()",
                            desc: "Jumps the currently playing level sequence to its end, triggering `OnSequenceFinished` as if it completed naturally. Respects `FNpcDialogSequence::bSkippable`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "NewSequence(FNpcDialogSequence Sequence)",
                            desc: "Called by `UNpcDialogComponent` when a new sequence begins. Updates `ActiveSequence`, resets the subtitle index, and broadcasts `OnNewSequenceDelegate`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "SequenceFinished(FNpcDialogSequence Sequence)",
                            desc: "Called by `UNpcDialogComponent` when the active sequence ends. Broadcasts `OnSequenceFinishedDelegate` and triggers any deferred script events queued for end-of-tree execution.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "IsDialogActive() : bool",
                            desc: "Returns `true` while a dialog interaction is in progress. Use to gate input or disable other interactions.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetActiveDialogNPC() : AActor*",
                            desc: "Returns the NPC actor currently engaged in dialog, or `nullptr` if no dialog is active.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog Manager", cls: "meta-cat" }]
                        }
                    ]
                },
                {
                    title: "Subtitle & Standalone Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "NotifySubtitleFilePlaying(USoundBase* SoundFile)",
                            desc: "Call this from the NPC Blueprint (or animation notify) when dialog audio begins. Broadcasts `OnSubtitleAudioPlay` so UI can synchronise captions to audio.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Subtitles", cls: "meta-cat" }]
                        },
                        {
                            sig:  "DialogSubtitleMarkerHit(const int Index, const FString Speaker)",
                            desc: "Forwarded from `UNpcDialogComponent` when a subtitle marker frame is crossed. Updates `CurrentSubtitleIndex` and broadcasts `OnSubtitleMarkerHit`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Subtitles", cls: "meta-cat" }]
                        },
                        {
                            sig:  "TriggerStandaloneDialogSequence(const FStandaloneDialogSequence& SequenceData)",
                            desc: "Triggers a self-contained level sequence outside a dialog tree. Broadcasts `OnStandaloneSequenceTriggered` with the full sequence data for UI handling.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Standalone Event", cls: "meta-cat" }]
                        },
                        {
                            sig:  "StandaloneDialogSequenceEnded()",
                            desc: "Call when a standalone sequence has finished playback. Broadcasts `OnStandaloneSequenceEnded`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Standalone Event", cls: "meta-cat" }]
                        },
                        {
                            sig:  "DialogFailedToStart(AActor* InvokingActor, const FString& Reason)",
                            desc: "Called by the owning NPC component when a dialog tree fails to start. Broadcasts `OnDialogFailedToStart` and notifies the invoking actor via `IDialogInterface::OnFailedToPlayDialogTree()`.",
                            meta: [{ text: "Dialog Manager", cls: "meta-cat" }]
                        }
                    ]
                }
            ]
        },

        // ── UNpcDialogComponent ────────────────────────────────────────────────
        {
            id:          "UNpcDialogComponent",
            name:        "UNpcDialogComponent",
            parents:     ["UActorComponent"],
            navDot:      "dot-component",
            description: "Actor component attached to NPC actors. Owns the `UDialogTree` reference, maintains a per-instance copy of dialog state modifiers, manages level sequence playback via `ULevelSequencePlayer`, drives the subtitle marker pipeline, and serves as the liaison between the NPC and `UDialogManagerSubsystem`. Subclass in C++ and override `OnBeginDialog()` for specialised NPC behaviour.",
            badges: [
                { text: "ActorComponent",    cls: "badge-component" },
                { text: "BlueprintSpawnable", cls: "badge-blueprintable" }
            ],
            body: [
                {
                    title: "Public Properties",
                    type:  "propertyTable",
                    rows: [
                        {
                            name: "DialogTree", type: "UDialogTree*",
                            desc: "The data asset containing all sequences, choices, conditions, and events for this NPC. Assign in the Details panel. Can be swapped at runtime via `ChangeDialogTree()`.",
                            meta: [{ text: "Dialog", cls: "meta-cat" }, { text: "BlueprintReadWrite", cls: "meta-bp" }]
                        }
                    ]
                },
                {
                    title: "Public Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "BeginInteract()",
                            desc: "Entry point for starting an NPC dialog interaction. Evaluates all `bConsideredForDialogStart` sequences, selects the highest-priority qualified one, and kicks off playback. Does nothing if `CanStartADialog()` returns false.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "CanStartADialog() : bool",
                            desc: "Returns true if `DialogTree` is set and at least one sequence qualifies. Call this before `BeginInteract()` to decide whether to show an interaction prompt.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "CheckDialogCondition(const FDialogCondition& Condition) : bool",
                            desc: "Evaluates a single `FDialogCondition` against current game state. Routes to the appropriate sub-check (quest, inventory, state, reputation) based on `Condition.TargetSystem`. Applies `bNegate` to the result.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "FilterQualifiedChoices() : TArray<FDialogChoice>",
                            desc: "Convenience wrapper that calls `GetValidDialogChoices(CurrentDialogSequence)`. Returns only choices whose `QualifyingConditions` all pass.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "PlaySequenceByGuid(FGuid Id)",
                            desc: "Looks up the sequence with the matching GUID in `DialogTree->Sequences` and begins playback. Updates `CurrentDialogSequence` and notifies the subsystem via `NewSequence()`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "StopCurrentSequence(const bool bCallSequenceFinished)",
                            desc: "Immediately stops the active level sequence player. Pass `true` to fire `OnSequenceFinished` as if it ended naturally (useful for skipping); pass `false` to terminate silently.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "ChangeDialogTree(UDialogTree* NewDialogTree, const bool bCopyMatchingStates = false)",
                            desc: "Swaps the active dialog tree at runtime. If `bCopyMatchingStates` is true, any state keys that exist in both the old and new tree are copied across, preserving progress.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "AddDialogState(const FName Key, const bool Value)",
                            desc: "Inserts or updates a key-value entry in the component's local `DialogStateModifiers` map. This instance map takes precedence over the data asset's base states.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Dialog", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetDialogTree() : UDialogTree*",
                            desc: "Returns the currently assigned `UDialogTree` data asset.",
                            meta: [{ text: "C++ API", cls: "meta-prot" }]
                        }
                    ]
                },
                {
                    title: "Protected / Override Points",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "OnBeginDialog() [virtual]",
                            desc: "Called internally at the moment dialog starts. Override in a C++ subclass to execute logic that must run exactly once at dialog initialisation — e.g. disabling NPC movement, playing an idle animation, or logging analytics.",
                            meta: [{ text: "C++ Override", cls: "meta-prot" }]
                        }
                    ]
                }
            ]
        },

        // ── UDialogTree ────────────────────────────────────────────────────────
        {
            id:          "UDialogTree",
            name:        "UDialogTree",
            parents:     ["UDataAsset"],
            navDot:      "dot-actor",
            description: "Data asset that stores the complete dialog data for one NPC or conversation context. All maps are keyed by `FGuid` so individual elements can be safely referenced without index fragility. Assign to `UNpcDialogComponent::DialogTree`. In the editor the asset opens as a visual graph.",
            badges: [
                { text: "DataAsset",  cls: "badge-actor" },
                { text: "BlueprintType", cls: "badge-blueprintable" }
            ],
            body: [
                {
                    title: "Properties",
                    type:  "propertyTable",
                    rows: [
                        {
                            name: "Sequences", type: "TMap<FGuid, FNpcDialogSequence>",
                            desc: "All dialog sequences belonging to this tree, keyed by their `SequenceId`. The graph editor writes directly to this map.",
                            meta: [{ text: "Sequences", cls: "meta-cat" }, { text: "BlueprintReadOnly", cls: "meta-bp" }]
                        },
                        {
                            name: "DialogChoices", type: "TMap<FGuid, FDialogChoice>",
                            desc: "All player choices defined in this tree, keyed by `DialogChoiceId`. Sequences reference choices by GUID.",
                            meta: [{ text: "Choices", cls: "meta-cat" }, { text: "BlueprintReadOnly", cls: "meta-bp" }]
                        },
                        {
                            name: "Conditions", type: "TMap<FGuid, FDialogCondition>",
                            desc: "All conditions defined in this tree. Sequences and choices reference conditions by GUID through their `QualifyingConditions` arrays.",
                            meta: [{ text: "Conditions", cls: "meta-cat" }, { text: "BlueprintReadOnly", cls: "meta-bp" }]
                        },
                        {
                            name: "Events", type: "TMap<FGuid, FDialogEventData>",
                            desc: "All events (quest, inventory, state, reputation, script) defined in this tree. Sequences and choices reference events by GUID through their `TriggeredEvents` arrays.",
                            meta: [{ text: "Events", cls: "meta-cat" }]
                        },
                        {
                            name: "DialogStates", type: "TMap<FName, bool>", default: "{}",
                            desc: "Base dialog state key-value pairs for this tree. At runtime the component clones this into `DialogStateModifiers` per instance so the asset is never mutated. State conditions reference keys defined here.",
                            meta: [{ text: "Dialog States", cls: "meta-cat" }, { text: "BlueprintReadWrite", cls: "meta-bp" }]
                        }
                    ]
                },
                {
                    title: "Notes",
                    type:  "calloutGroup",
                    callouts: [
                        {
                            type: "note",
                            text: "`EditorNodeLayout` and `EditorGraph` are editor-only (`WITH_EDITORONLY_DATA`) and stripped from shipping builds. Never access them in runtime code."
                        }
                    ]
                }
            ]
        },

        // ── UDialogScriptEventBase ─────────────────────────────────────────────
        {
            id:          "UDialogScriptEventBase",
            name:        "UDialogScriptEventBase",
            parents:     ["UObject"],
            navDot:      "dot-component",
            description: "Base class for custom game-logic scripts that are spawned and executed by the dialog system. Subclass in Blueprint, override `ExecuteScript`, and assign the subclass to a `FDialogEventData::ScriptClass` field on the relevant event node. Scripts can optionally be deferred until the entire dialog tree ends. **Must** call `FinishExecute()` when complete.",
            badges: [
                { text: "Blueprintable", cls: "badge-blueprintable" }
            ],
            body: [
                {
                    title: "Properties",
                    type:  "propertyTable",
                    rows: [
                        {
                            name: "bDeferExecuteToEndOfDialogTree", type: "bool", default: "false",
                            desc: "When true, the script is queued and not executed until `EndCurrentDialog()` is called. When false (default), the script executes immediately when its associated event fires during the dialog tree.",
                            meta: [{ text: "Execution", cls: "meta-cat" }]
                        }
                    ]
                },
                {
                    title: "Public Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "ExecuteScript(AActor* Instigator, UWorld* World) [BlueprintImplementableEvent]",
                            desc: "Override in Blueprint to implement the script's game logic. `Instigator` is the NPC actor that owns the dialog component. `World` is provided as a convenience context object.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }, { text: "Script", cls: "meta-cat" }]
                        },
                        {
                            sig:  "FinishExecute()",
                            desc: "**Must be called** when the script has finished all of its logic. Fires `OnScriptEventFinished`, which triggers `UDialogManagerSubsystem::OnScriptEnded()` to remove this instance from the active script list.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Script", cls: "meta-cat" }]
                        },
                        {
                            sig:  "InitBlueprint(AActor* Instigator)",
                            desc: "Blueprint-accessible version of `NativeExecute`. Call this if you need to manually trigger a script from a Blueprint flow rather than through the standard event pipeline.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Script", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetWorldContext() : UWorld*",
                            desc: "Returns the world object. Use inside `ExecuteScript` to spawn actors, run timers, or call world-level APIs.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "BlueprintPure", cls: "meta-pure" }, { text: "Script", cls: "meta-cat" }]
                        }
                    ]
                },
                {
                    title: "Protected Helpers",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "SpawnActor(TSubclassOf<AActor> Actor, FTransform Transform) : AActor*",
                            desc: "Convenience function to spawn an actor into the world at the given transform. Wraps `GetWorld()->SpawnActor()`.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Helpers", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetPlayerPawn(const int ControllerIndex) : APawn*",
                            desc: "Returns the player pawn for the given controller index. Index 0 is the primary local player.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "BlueprintPure", cls: "meta-pure" }, { text: "Helpers", cls: "meta-cat" }]
                        },
                        {
                            sig:  "GetPlayerController(const int ControllerIndex) : APlayerController*",
                            desc: "Returns the player controller for the given controller index.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "BlueprintPure", cls: "meta-pure" }, { text: "Helpers", cls: "meta-cat" }]
                        }
                    ]
                }
            ]
        },

        // ── IDialogInterface ───────────────────────────────────────────────────
        {
            id:          "IDialogInterface",
            name:        "IDialogInterface",
            parents:     ["UInterface"],
            navDot:      "dot-actor",
            description: "Core dialog lifecycle interface. Implement on both the **player pawn** and the **NPC actor** to receive automatic notifications when dialog interactions start, end, or fail — without needing to manually bind delegates. All functions are `BlueprintImplementableEvent`, meaning implementation is done entirely in Blueprint.",
            badges: [
                { text: "Interface", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Interface Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "BeginDialogTreeInteraction() [BlueprintImplementableEvent]",
                            desc: "Called on both the player pawn and the NPC when a dialog tree interaction is registered. Use to disable movement, play a greeting animation, or open the dialog UI.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "EndDialogTreeInteraction() [BlueprintImplementableEvent]",
                            desc: "Called on both the player pawn and the NPC when the dialog interaction ends cleanly. Use to re-enable movement and close the dialog UI.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "DialogSequenceInitalised(FNpcDialogSequence Sequence) [BlueprintImplementableEvent]",
                            desc: "Called on both actors when a specific sequence begins playing. Provides the full sequence struct for per-sequence setup logic.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "DialogSequenceEnded(FNpcDialogSequence Sequence) [BlueprintImplementableEvent]",
                            desc: "Called on both actors when a level sequence finishes. Provides the completed sequence struct.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "OnFailedToPlayDialogTree(const FString& Reason) [BlueprintImplementableEvent]",
                            desc: "Called on the invoking NPC actor when dialog fails to start. `Reason` contains a diagnostic message. Use to play a fallback animation or log a player-facing message.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        }
                    ]
                }
            ]
        },

        // ── IDialogQuestInterface ──────────────────────────────────────────────
        {
            id:          "IDialogQuestInterface",
            name:        "IDialogQuestInterface",
            parents:     ["UInterface"],
            navDot:      "dot-actor",
            description: "Implement on the **player pawn** to allow the dialog system to query quest state and issue quest mutations (add, remove, complete, fail, progress). The dialog system calls these functions automatically when quest conditions are evaluated or quest events are executed.",
            badges: [
                { text: "Interface", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Query Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "HasQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player currently has the active quest identified by `QuestId`.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "CompletedQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player has the quest in their completed quests list. Should not check if it is currently active.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "IsQuestReadyForTurnIn(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player has the active quest and all its objectives are complete (ready to hand in).",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "HasFailedQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player has the quest in their failed quests list.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        }
                    ]
                },
                {
                    title: "Mutation Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "AddQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Add the quest to the player's active quest log. Called when a dialog event of type `ADD_QUEST` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "RemoveQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Remove the quest from the player's active quest log. Called when a dialog event of type `REMOVE_QUEST` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "CompleteQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Mark the quest as complete. Called when a dialog event of type `COMPLETE_QUEST` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "FailQuest(const FName QuestId) : bool [BlueprintImplementableEvent]",
                            desc: "Mark the quest as failed. Called when a dialog event of type `FAIL_QUEST` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "QuestDialogSelected(FName EventId) : bool [BlueprintImplementableEvent]",
                            desc: "Called when a quest-related dialog option is selected and a `PROGRESS_QUEST` event fires. `EventId` identifies the specific objective to progress.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        }
                    ]
                }
            ]
        },

        // ── IDialogInventoryInterface ──────────────────────────────────────────
        {
            id:          "IDialogInventoryInterface",
            name:        "IDialogInventoryInterface",
            parents:     ["UInterface"],
            navDot:      "dot-actor",
            description: "Implement on the **player pawn** to allow the dialog system to query inventory state and add or remove items. Used for `EDialogConditionRequirementType::HAVE` and `AT_LEAST` conditions targeting `EDialogSystemTarget::INVENTORY`, and for `ADD_ITEM` / `REMOVE_ITEM` event actions.",
            badges: [
                { text: "Interface", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Interface Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "HasItem(const FName ItemId) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player has at least one of the item identified by `ItemId`.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "HasNumItem(const FName ItemId, const int Num) : bool [BlueprintImplementableEvent]",
                            desc: "Return true if the player has at least `Num` of the item identified by `ItemId`. Used for `AT_LEAST` conditions with a `Value` > 1.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "AddItem(const FName ItemId, const int NumItems) : bool [BlueprintImplementableEvent]",
                            desc: "Add `NumItems` of the item to the player's inventory. Called when a dialog event of type `ADD_ITEM` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "RemoveItem(const FName ItemId, const int NumItems) : bool [BlueprintImplementableEvent]",
                            desc: "Remove `NumItems` of the item from the player's inventory. Called when a dialog event of type `REMOVE_ITEM` fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        }
                    ]
                }
            ]
        },

        // ── IDialogReputationInterface ─────────────────────────────────────────
        {
            id:          "IDialogReputationInterface",
            name:        "IDialogReputationInterface",
            parents:     ["UInterface"],
            navDot:      "dot-actor",
            description: "Implement on the **player pawn** (or another relevant actor) to allow the dialog system to read and modify reputation values. Used by `AT_LEAST` and `LESS_THAN` conditions targeting `EDialogSystemTarget::REPUTATION` and by `MODIFY_REPUTATION` event actions.",
            badges: [
                { text: "Interface", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Interface Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "GetReputation(const FName ReputationType) : int32 [BlueprintImplementableEvent]",
                            desc: "Return the current reputation value for the given `ReputationType` key. Called during condition evaluation.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        },
                        {
                            sig:  "ModifyReputation(const FName ReputationType, const int32 Amount) [BlueprintImplementableEvent]",
                            desc: "Add `Amount` to the reputation value for `ReputationType` (negative values reduce it). Called when a `MODIFY_REPUTATION` event fires.",
                            meta: [{ text: "BlueprintImplementableEvent", cls: "meta-impl" }]
                        }
                    ]
                }
            ]
        },

        // ── URpgDialogFunctionLibrary ──────────────────────────────────────────
        {
            id:          "URpgDialogFunctionLibrary",
            name:        "URpgDialogFunctionLibrary",
            parents:     ["UBlueprintFunctionLibrary"],
            navDot:      "dot-actor",
            description: "Blueprint function library providing static utility functions for the RpgDialogSystem plugin. Currently contains tag conversion helpers used in inventory condition evaluation.",
            badges: [
                { text: "FunctionLibrary", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Functions",
                    type:  "functionTable",
                    rows: [
                        {
                            sig:  "TryConvertNameToGameplayTag(FName Name, FGameplayTag& OutTag) : bool",
                            desc: "Attempts to convert an `FName` to an `FGameplayTag`. Returns true and sets `OutTag` if the name maps to a valid registered tag. Used internally when inventory conditions compare against Gameplay Tag-based item IDs.",
                            meta: [{ text: "BlueprintCallable", cls: "meta-bp" }, { text: "Inventory|Tags", cls: "meta-cat" }]
                        }
                    ]
                }
            ]
        },

        // ── DialogTypes ────────────────────────────────────────────────────────
        {
            id:          "DialogTypes",
            name:        "DialogTypes",
            parents:     [],
            navDot:      "dot-actor",
            description: "Core type definitions for the RpgDialogSystem plugin. Contains all enums, structs, and helper classes used throughout the runtime. Defined in `DialogTypes.h`.",
            badges: [
                { text: "Types Header", cls: "badge-actor" }
            ],
            body: [
                {
                    title: "Enumerations",
                    type:  "enumBlock",
                    enums: [
                        {
                            name: "EDialogSystemTarget",
                            type: "uint8",
                            values: [
                                { name: "QUEST",      desc: "Targets the quest system. Conditions and events route to `IDialogQuestInterface`." },
                                { name: "INVENTORY",  desc: "Targets the inventory system. Conditions and events route to `IDialogInventoryInterface`." },
                                { name: "STATE",      desc: "Targets the local dialog state map on `UNpcDialogComponent`." },
                                { name: "SCRIPT",     desc: "Targets a custom `UDialogScriptEventBase` subclass for arbitrary logic." },
                                { name: "REPUTATION", desc: "Targets the reputation system. Conditions and events route to `IDialogReputationInterface`." }
                            ]
                        },
                        {
                            name: "EDialogActionType",
                            type: "uint8",
                            values: [
                                { name: "ADD_QUEST",         desc: "Adds a quest to the player's quest log via `IDialogQuestInterface::AddQuest()`." },
                                { name: "REMOVE_QUEST",      desc: "Removes a quest from the player's quest log." },
                                { name: "PROGRESS_QUEST",    desc: "Progresses a quest objective via `IDialogQuestInterface::QuestDialogSelected()`." },
                                { name: "COMPLETE_QUEST",    desc: "Marks a quest as completed." },
                                { name: "FAIL_QUEST",        desc: "Marks a quest as failed." },
                                { name: "ADD_ITEM",          desc: "Adds N items to the player's inventory via `IDialogInventoryInterface::AddItem()`." },
                                { name: "REMOVE_ITEM",       desc: "Removes N items from the player's inventory." },
                                { name: "SET_STATE",         desc: "Sets a key-value pair in the NPC component's dialog state map." },
                                { name: "CUSTOM_SCRIPT",     desc: "Spawns and executes a `UDialogScriptEventBase` subclass." },
                                { name: "MODIFY_REPUTATION", desc: "Modifies a reputation value via `IDialogReputationInterface::ModifyReputation()`." }
                            ]
                        },
                        {
                            name: "EDialogConditionRequirementType",
                            type: "uint8",
                            values: [
                                { name: "HAVE",          desc: "Player must possess the target item or have the target quest active." },
                                { name: "IS_COMPLETE",   desc: "Target quest must be in a ready-to-turn-in state (all objectives done)." },
                                { name: "HAS_COMPLETED", desc: "Target quest must be in the player's completed quests history." },
                                { name: "HAS_FAILED",    desc: "Target quest must be in the player's failed quests history." },
                                { name: "AT_LEAST",      desc: "Numeric check: player must have at least `Value` of the target (items or reputation)." },
                                { name: "LESS_THAN",     desc: "Numeric check: player must have strictly less than `Value` of the target (items or reputation)." }
                            ]
                        },
                        {
                            name: "EQuestState",
                            type: "uint32",
                            values: [
                                { name: "NOT_STARTED", desc: "Quest has not been started by the player." },
                                { name: "IN_PROGRESS", desc: "Quest is active and objectives are not yet complete." },
                                { name: "COMPLETED",   desc: "Quest has been completed." }
                            ]
                        }
                    ]
                },
                {
                    title: "Structs",
                    type:  "structBlock",
                    structs: [
                        {
                            name: "FNpcDialogSequence",
                            fields: [
                                { name: "SequenceId : FGuid",                          desc: "Unique identifier for this sequence. Used as the map key in `UDialogTree::Sequences` and as the target of `NextSequenceId` references." },
                                { name: "bConsideredForDialogStart : bool",             desc: "If true, this sequence is evaluated as a candidate when `BeginInteract()` is called. Set false for sequences that should only be reached via player choice." },
                                { name: "SelectionPriority : int",                     desc: "When multiple sequences qualify at dialog start, the one with the highest priority is selected. Default 1." },
                                { name: "QualifyingConditions : TArray<FGuid>",        desc: "GUIDs of `FDialogCondition` entries that must all pass for this sequence to be considered at dialog start. Only evaluated when `bConsideredForDialogStart` is true." },
                                { name: "LevelSequence : TSoftObjectPtr<ULevelSequence>", desc: "The cinematic to play when this sequence is selected. Streamed asynchronously." },
                                { name: "Subtitles : TArray<FText>",                   desc: "Subtitle strings displayed in order as subtitle frame markers are hit during playback." },
                                { name: "DialogChoices : TArray<FGuid>",               desc: "GUIDs of `FDialogChoice` entries presented to the player after the sequence ends." },
                                { name: "AutoSelectNextSequence : FGuid",              desc: "If valid, the dialog system automatically advances to this sequence ID after the current one ends, bypassing player choice." },
                                { name: "TriggeredEvents : TArray<FGuid>",             desc: "GUIDs of `FDialogEventData` entries executed when this sequence begins playing." },
                                { name: "DialogEndScriptEvent : TSoftClassPtr<UDialogScriptEventBase>", desc: "Script class to execute when the dialog tree ends. Overridden by later sequences in the same conversation that also specify a script." },
                                { name: "bSkippable : bool",                           desc: "Whether the player can skip this sequence. Default true." }
                            ]
                        },
                        {
                            name: "FDialogChoice",
                            fields: [
                                { name: "DialogChoiceId : FGuid",              desc: "Unique identifier for this choice. Key in `UDialogTree::DialogChoices`." },
                                { name: "DisplayText : FText",                 desc: "The text shown to the player in the choice UI." },
                                { name: "QualifyingConditions : TArray<FGuid>", desc: "GUIDs of conditions that must all pass for this choice to be presented. Evaluated by `FilterQualifiedChoices()`." },
                                { name: "NextSequenceId : FGuid",              desc: "The sequence to play when this choice is selected. Invalid GUID means the conversation ends." },
                                { name: "TriggeredEvents : TArray<FGuid>",     desc: "GUIDs of events executed when this choice is submitted via `SelectDialogChoice()`." }
                            ]
                        },
                        {
                            name: "FDialogCondition",
                            fields: [
                                { name: "ConditionId : FGuid",                           desc: "Unique identifier. Key in `UDialogTree::Conditions`." },
                                { name: "ConditionType : EDialogConditionRequirementType", desc: "The comparison type: HAVE, IS_COMPLETE, HAS_COMPLETED, HAS_FAILED, AT_LEAST, or LESS_THAN." },
                                { name: "TargetSystem : EDialogSystemTarget",            desc: "Which system to query: QUEST, INVENTORY, STATE, or REPUTATION." },
                                { name: "DataId : FName",                                desc: "The ID of the target data row — e.g. a quest row name, item ID, or state key." },
                                { name: "bNegate : bool",                                desc: "If true, the evaluated result is inverted. HAVE becomes NOT_HAVE, etc." },
                                { name: "Value : int",                                   desc: "Numeric threshold for AT_LEAST and LESS_THAN comparisons, and for item count checks. Default 1. Clamped to minimum 1." }
                            ]
                        },
                        {
                            name: "FDialogEventData",
                            fields: [
                                { name: "DialogEventId : FGuid",                          desc: "Unique identifier. Key in `UDialogTree::Events`." },
                                { name: "TargetSystem : EDialogSystemTarget",             desc: "Which system this event targets: QUEST, INVENTORY, STATE, SCRIPT, or REPUTATION." },
                                { name: "EventAction : EDialogActionType",                desc: "The action to perform on the target system." },
                                { name: "ScriptClass : TSoftClassPtr<UDialogScriptEventBase>", desc: "The script class to spawn and execute. Only used when `TargetSystem` is SCRIPT." },
                                { name: "DataId : FName",                                 desc: "The ID of the target data — quest ID, item ID, state key, or reputation type." },
                                { name: "Value : int",                                    desc: "Numeric argument — item count for inventory events, reputation amount for reputation events." }
                            ]
                        },
                        {
                            name: "FStandaloneDialogSequence",
                            fields: [
                                { name: "LevelSequence : TSoftObjectPtr<ULevelSequence>", desc: "The level sequence to play as a standalone cinematic, outside of any dialog tree." },
                                { name: "Subtitles : TArray<FText>",                      desc: "Subtitle strings for the standalone sequence, displayed as subtitle markers are hit." }
                            ]
                        }
                    ]
                }
            ]
        }

    ], // end classes

    // ── DELEGATES REFERENCE ───────────────────────────────────────────────────
    delegatesRef: [
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewSequence, const FNpcDialogSequence&, Sequence)",
            desc: "Broadcast by `UDialogManagerSubsystem` when a new sequence begins playing."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSequenceFinished, const FNpcDialogSequence&, Sequence)",
            desc: "Broadcast by `UDialogManagerSubsystem` when the active sequence finishes."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSubtitleMarkerHit, const int, Index, const FString, Speaker)",
            desc: "Broadcast when a subtitle frame marker is hit. `Index` maps into the sequence's `Subtitles` array; `Speaker` is the marker label string."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogEnded)",
            desc: "Broadcast when the active dialog interaction ends."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogStarted)",
            desc: "Broadcast when a new dialog interaction is registered."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubtitleDialogPlay, USoundBase*, SoundFile)",
            desc: "Broadcast when NPC dialog audio begins playing. Passes the sound asset for UI synchronisation."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogFailedToStart, const FString&, Reason)",
            desc: "Broadcast when a dialog tree fails to start. Passes a diagnostic reason string."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStandaloneSequenceTriggered, const FStandaloneDialogSequence&, SequenceData)",
            desc: "Broadcast when a standalone dialog sequence is triggered outside a dialog tree."
        },
        {
            sig:  "DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStandaloneSequenceEnded)",
            desc: "Broadcast when a standalone dialog sequence finishes."
        },
        {
            sig:  "DECLARE_DELEGATE_OneParam(FOnDialogScriptEventFinished, UDialogScriptEventBase*)",
            desc: "Internal delegate on `UDialogScriptEventBase`. Fired by `FinishExecute()` to notify the subsystem that a script instance has completed."
        }
    ]

}; // end window.PLUGIN_DATA
