## electron-share-memory

Shared memory between processes.

The current version only works on windows.

## Usage

```js
const memory = require('electron-share-memory')

//Process A set memory
memory.SetShareMemory(name, maxSize, dataSize, data)

//Process B get memory
let data = memory.GetShareMemory(name)

//Clear for both process
memory.ClearShareMemory()

```

## License
MIT, please see LICENSE for details. Copyright (c) 2020 weedsboy.