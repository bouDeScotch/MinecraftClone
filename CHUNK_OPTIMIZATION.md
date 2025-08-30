# Chunk File Size Optimization

This document describes the optimizations implemented to dramatically reduce chunk file sizes in the MinecraftClone project.

## Summary of Improvements

The chunk saving format has been optimized to achieve **97.2% file size reduction** through the following techniques:

### Original Format Issues
- Each Block struct stored full position (12 bytes) + type (4 bytes) = 16 bytes per block
- File format: chunk position + block count + array of Block structs
- Typical file size: ~217KB for a 41% filled chunk

### Optimization 1: Remove Position Redundancy
- Positions can be calculated from array index using: `index % width`, `(index/width) % height`, `index/(width*height)`
- Store only: block index (4 bytes) + block type (1 byte) = 5 bytes per block
- **Reduction: 68.7%**

### Optimization 2: Run-Length Encoding (RLE)
- Minecraft terrain has many consecutive blocks of the same type (stone layers, dirt layers, etc.)
- Store: starting index + block type + run length
- Format: 4 bytes (index) + 1 byte (type) + 4 bytes (length) = 9 bytes per run
- **Total reduction: 97.2%** (with 91.1% additional reduction over simple optimization)

## File Format Versions

The implementation supports backward compatibility with three format versions:

### Version 1 (Original)
```
chunk_position (12 bytes)
non_air_count (4 bytes)
for each non-air block:
    full_block_struct (16 bytes)
```

### Version 2 (Simple Optimized) 
```
version_number (4 bytes)
chunk_position (12 bytes)
non_air_count (4 bytes)
for each non-air block:
    block_index (4 bytes)
    block_type (1 byte)
```

### Version 3 (RLE Compressed)
```
version_number (4 bytes)
chunk_position (12 bytes) 
compressed_run_count (4 bytes)
for each run:
    start_index (4 bytes)
    block_type (1 byte)
    run_length (4 bytes)
```

## Performance Results

Test with realistic terrain (41.3% filled chunk, 13,535 non-air blocks):

| Format | File Size | Reduction |
|--------|-----------|-----------|
| Original | 216,576 bytes | - |
| Simple Optimized | 67,695 bytes | 68.7% |
| RLE Compressed | 6,005 bytes | 97.2% |

## Implementation Notes

- **Backward Compatibility**: Loader automatically detects and handles all three formats
- **Memory Efficiency**: Block positions are recalculated on load, not stored redundantly
- **Data Types**: Uses `uint8_t` for block types (sufficient for 9 block types) and `uint32_t` for indices
- **Error Handling**: Includes bounds checking and file validation
- **No External Dependencies**: Uses only standard C++ libraries

## Benefits

1. **Storage Savings**: Up to 97% reduction in disk usage for world saves
2. **Network Efficiency**: Dramatically reduced bandwidth for multiplayer chunk transmission  
3. **Loading Performance**: Smaller files load faster from disk
4. **Memory Efficiency**: Removes redundant position storage in memory
5. **Backward Compatibility**: Existing saves continue to work without conversion

The RLE compression is particularly effective for Minecraft-style terrain due to the natural occurrence of large continuous regions of the same block type (bedrock layers, stone formations, dirt layers, etc.).