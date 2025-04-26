
### Manifest

```xml
<!-- app.vcxproj -->

<ItemDefinitionGroup>
  <PostBuildEvent>
    <Command>mt.exe -manifest "$(ProjectDir)resources/.manifest" -outputresource:"$(TargetPath);#1"</Command>
  </PostBuildEvent>
</ItemDefinitionGroup>
```
