## Summary

<!-- What does this PR change? Keep the first sentence clear enough to understand from the PR list. -->

## Why

<!-- Why is this change needed? Link the related issue if one exists, for example: Closes #25. -->

## Type of change

- [ ] Gameplay or simulation logic
- [ ] Island generation, tile-map, or territory expansion
- [ ] Biome, resource, monster, or building rules
- [ ] Combat, quest, or exploration content
- [ ] Automation or resource-flow behavior
- [ ] Client UI, controls, camera, or player feedback
- [ ] Server, networking, persistence, or shared protocol
- [ ] Assets, content, balancing, or tuning
- [ ] Build, tooling, docs, or maintenance

## Player-facing impact

<!-- Describe changes to the player's experience, island growth, biome access, progression, balance, difficulty, or performance. Use "None" if this is internal-only. -->

## World and biome impact

<!-- Fill in affected rows. Use "N/A" for rows that do not apply. -->

- Starting island generation:
- Territory expansion blocks:
- Biome distribution or selection:
- Resource availability:
- Monster spawning:
- Buildable structures:
- Save/load compatibility:

## Technical notes

<!-- Mention important implementation details, tradeoffs, migration notes, or reviewer focus areas. -->

- Client impact:
- Server impact:
- EnTT/ECS impact:
- CMake/vcpkg impact:

## How was this tested?

<!-- List commands, platforms, manual scenarios, and any intentionally skipped checks. -->

- [ ] CMake configure/build succeeded
- [ ] Tests passed locally, or missing tests are explained below
- [ ] vcpkg dependencies restore/install correctly
- [ ] Changed gameplay or simulation behavior was manually tested
- [ ] Tile, island, and territory expansion interactions still behave as expected
- [ ] Biome-specific resources, monsters, and buildings still behave as expected
- [ ] Client/server behavior was checked, if affected
- [ ] Save/load or persistence behavior was checked, if affected
- [ ] Performance was considered for large islands, expanded maps, or busy automation setups

Test details:

```text
<!-- Example:
cmake -S . -B build
cmake --build build
ctest --test-dir build
Manual: started a new island, expanded into two biome blocks, verified resource and monster behavior.
-->
```

## Screenshots or recordings

<!-- Add screenshots, GIFs, or short recordings for visual/gameplay changes. Use "N/A" for non-visual changes. -->

## Checklist

- [ ] Documentation or README changes were added, if needed
- [ ] New dependencies, tools, assets, or configuration changes are documented
- [ ] Generated files, local-only settings, secrets, and credentials were not introduced
- [ ] Breaking changes, save migrations, or protocol changes are clearly described
- [ ] Review notes are included for risky or non-obvious changes
