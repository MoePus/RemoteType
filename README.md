# RemoteType
A C++ Header only lib to help to write simple code to get remote class.

## How To Use
1. Include RemoteType.h
2. Implement your RPM method
3. Define Class with RemoteType

## Showcase
```C++
  class ULevel :public UObject
  {
    public:
      UINT8 DONTCARE[152-sizeof(UObject)];
      TArray<AActor*> Actors;
      TArray<AActor*> ActorsForGC;
  }
  
  class AActor : public UObject
  {
    public:
      UINT8 DONTCARE[0x140 - sizeof(UObject)];
      ReflectPointer <APawn*> Instigator;
      TArray<AActor*> Children;
      ReflectPointer <USceneComponent*> RootComponent;
   };
```

```C++
  for (auto levelIndex = 1; levelIndex < levels.Num(); levelIndex++)
  {
  	auto level = levels.get(levelIndex);
  	auto liveActors = level->findLiveActors();

  	for (ReflectPointer<AActor*> actor_r : liveActors)
  	{
  		auto actor = actor_r.share();

  		if (!actor->Outer)
  			continue;
  			
  		auto RootComponent = actor->RootComponent.share();
  		auto pos = RootComponent->ComponentToWorld.Translation;
  		...
  	}
  }
```
