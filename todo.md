# Right now

### Refactor main.cpp 
clear it after multithread branch merge

- remove commentaries

- Hide DE depended things

- Hide DesktopEnv inserting branches (if useDB...) inside 

### Sqlite optimizations

- Add BEGIN, COMMIT

- On dump, reuest all app ids by name at once (mb try more benchmarks)

- Test sync turn off

- Test journal mode e.g. WALL

### Data proccessing

- !!!!!!!Minimal interval berween app changing (1sec by default). I have to parse app name anyway (to save last opened app). (Every record MUST have unique date)

# One day...

### Proccessed statistics

- Implement user. Needed for sync between databases

- Think about placing filler record on: 
    - empty name application
    - start and end recording

- Session locks

- Media (listen playerctl)

- Think about browsers (insert in separate table or what)

### Sqlite 

- Test shared cache 

- Test in-memory db instead of RAM storage

- Test static typization

- Implement database merging with user checking: if user exist, notify and check bounds (get first and last date from passed table and check is there dates between in main table. If not, check every date, idk)

### Modding

- Create module class that listened in one of threads (idle as shared objects (dll))

### Gui

... )
