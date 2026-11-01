# LOB-engine
Limit Order Book personnal project

## Compilation guide:

```
cmake -S . -B build
cmake --build build
./build/itchparse -f fichier
```

for sanitizers : 
```
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```


## J1 — ITCH 5.0 parser

Iterate a NASDAQ TotalView-ITCH 5.0 archive without cursor drift, histogram
messages by type, and establish the throughput baseline that later
milestones are measured against.

### Data

NASDAQ BX TotalView-ITCH 5.0, 2019-12-30 session
([source](https://emi.nasdaq.com/ITCH/Nasdaq%20BX%20ITCH/)): 862,946,629 B
uncompressed, 29,156,757 messages, 29.6 B/message on average.

Note that the archive framing is not part of the ITCH spec — each message is
preceded by a 2-byte big-endian length, standing in for the delimitation
MoldUDP64 provides on the live multicast feed. This parser consumes a
recording format, not the wire format.

### Design

The decoder splits the stream and hands the consumer a `span` bounded to the
current message. It knows no message type and keeps no state beyond the
cursor; nothing is materialised, each message lives for one loop iteration.
The J1 consumer increments a 256-entry table indexed by the type byte —
branchless, 2 KB resident in L1. Keeping the split here means the parsing
path stays byte-identical across milestones and cannot contaminate the later
comparison.

### Correctness

Bytes consumed match the file size exactly (all 862,946,629).
This is the primary check: a single-byte drift desynchronises the cursor
permanently, with no possible resynchronisation.

Three weaker checks agree. No unknown type appears. `S` = 6, which is the
exact shape of a session — start of messages, start/end of system hours,
start/end of market hours, end of messages. And the histogram is plausible:
`D/A` = 0.97, so 97% of added orders are cancelled, while executions
(`E` + `C` = 581,525) are 20x rarer than adds.

Ten fixtures cover the boundaries — empty file, truncated prefix, truncated
body, trailing orphan byte, unknown type — against expected outputs derived
from the format rather than from the binary. The suite also runs under
ASan/UBSan.

### Measurement

i7-13700HX (16C/24T, 5.0 GHz boost, 30 MB L3), 32 GB RAM, Linux 7.0,
g++ 13.3, `-O2`. `steady_clock` around `parse()` only, warm page cache,
5 runs.

| | time | throughput | bandwidth | latency |
|---|---|---|---|---|
| min | 78.1 ms | 373.3 M msg/s | 11.05 GB/s | 2.68 ns/msg |
| median | 78.4 ms | 372.1 M msg/s | 11.01 GB/s | 2.69 ns/msg |
| max | 78.4 ms | 371.8 M msg/s | 11.00 GB/s | 2.69 ns/msg |

Spread min-to-max is 0.41%.

### What the numbers say

**The parser is memory-bandwidth bound, not compute bound.** 11 GB/s is the
practical ceiling here. The 862 MB buffer is 29x
larger than L3, and although the parser reads only ~3 bytes per message
(prefix and type byte), 64-byte cache lines force 13.5 M lines to be
streamed from DRAM. At 2.69 ns — roughly 13 cycles at boost clock — the
per-message work is entirely hidden behind that stream.

**Loading costs more than parsing.** `time` reports 454 ms for the full
program, 342 ms of it system time: the 862 MB copy from page cache into the
application buffer, at 2.5 GB/s. Parsing is 17% of the total.

### Decisions, and what is still open

**`read()` over `mmap`** — chosen for simplicity, and because a strictly
sequential access pattern is where kernel readahead performs best. The
measurement above partly invalidates that reasoning: the kernel-to-userspace
copy dominates, and `mmap` would remove it. Not yet quantified.

**Length prefix over a per-type table** — the prefix costs two loads plus a
byteswap, and creates a dependency: the CPU must resolve the length before
it knows where the next message starts. A `constexpr` table indexed by type
removes it. The prefix keeps two advantages, though: it allows skipping an
unknown type without derailing, and it cross-checks against drift. Intended
strategy is table in release, `prefix == table` assertion in debug. Not yet
quantified.

**`memcpy` + byteswap over struct casting** — casting is not viable here on
four counts: inverted byte order (no cast fixes that), compiler padding,
unaligned fields (the order reference sits at offset 11), and strict
aliasing. The real cost is nil, since the byteswap is required either way.


