# Right now

### Sqlite optimizations

- On dump, reuest all app ids by name at once (mb try more benchmarks)

- Test sync turn off

- Test journal mode e.g. WALL

### Data proccessing

- !!!!!!!Minimal interval berween app changing (1sec by default). I have to parse app name anyway (to save last opened app). (Every record MUST have unique date)
    - Why? Minimal interval may be good just just for less garbadge, i don't see why date have to be PK. 
        - Okey, see. It May have same title, but description 1, then 2, then 1. And this is not good for db

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

- Think about Modules future. One side i want to make it "one task" application, but i want to add media listener. 
    - Or keep external module for things like media
    - Or hardcode it and be happy

### Gui

... )
