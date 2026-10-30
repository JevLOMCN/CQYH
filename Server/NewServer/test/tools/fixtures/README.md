# Legacy tool fixtures

The three PAK files were generated on Windows x64 by the original `CPakFile`
implementation, using the deterministic data and timestamps in `test_pak.cpp`.
Only the CLI's removed `<hash_map>` dependency had been replaced before capture;
the archive writer, reader, hash and MD5 functions were unchanged. The linked
libraries are the CMake server libraries used by the tool targets.

Fixtures were captured immediately after creation. The old reader rewrote metadata
on close, including unused name offsets in unnamed archives. The new reader leaves
the original bytes alone. `named.pak` uses compressed names and metadata without a
hash table; `raw.pak` omits names and the hash table; `plain.pak` stores names,
metadata and its hash table without container compression. Each has a text file,
a deterministic 180,000-byte file spanning several payload chunks and an empty file.
Payload compression, filename hashes, MD5 values, timestamps and all archive bytes
are compared against these fixtures.

The UTF-16LE inputs contain existing and removed string-map entries, including a
duplicate final entry without a trailing newline. Variants cover a BOM and no BOM.
The three-byte truncated fixture deliberately ends halfway through a code unit.

`analysis-errors.txt` and `analysis-network.txt` use the server logger's original
timestamp, level and thread-ID prefix. Their `.expected` files were captured from
the original extraction functions using a standalone driver. That driver called
the parsers directly, avoiding the old worker startup's large fixed allocations
and its missing shutdown. The tests compare both CLI modes with these outputs,
then repeat using LF records and an unterminated final line.

`role-log-cases.json` freezes 42 results from the original role-log checker,
including 58 complete mismatch messages with role IDs, timestamps and amounts.
The cases cover addition/subtraction, non-gold events, ambiguous same-second
boundaries, several logout intervals, two-day input and deterministic generated
transactions. Only the original baseline driver's eager pool sizes were reduced
to 64 records, which is sufficient for these small inputs. The parser and matching
algorithm were unchanged. The modern tests also exercise previously unsafe
missing-source cases, malformed records, UTF-16LE inputs and 64-bit arithmetic.

`cash-original-format.txt` contains four CRLF report lines captured from the
original accounting tool's unchanged `_WriteLog` function with `TZ=GMT0`.
An isolated driver called that function without connecting to its configured
database. Cases include success/failure, negative balances, a percent-bearing
name and UTF-8 name bytes. The regression test compares these bytes with reports
generated through the modern accounting tool and the production SQL manager,
using a link-time client fake that opens no sockets. Additional fixtures cover
interleaved roles, self-transfers, signed amounts, full 32-bit IDs/timestamps,
64-bit accumulation, malformed rows, retries and resource cleanup. The obsolete
original database-manager interface was not used to claim a full baseline build.

`export-original-guild.txt` and `export-original-members.txt` were captured from
the original guild/member export functions through an isolated native-client fake.
Only an include path changed in the copied original source. The fixtures preserve
LF bytes from its binary-mode writers, row order, UTF-8/percent names and the
existing 32-byte username limit. Production SQL annotations now use signed BIGINT
for unsigned-domain IDs/timestamps and return compressed guild bytes for bounded
tool-owned decompression. SQL table/column selection is unchanged. The offline
tests validate these native bindings independently before comparing report bytes.

`export-original-role.txt` freezes the original role report header and printf
format strings using fixed, correctly typed values. The header-generating source
was extracted unchanged from the original callback. The isolated driver avoids
the old SQL decoder mismatch and gameworld initialization; it is a formatting
baseline, not a successful old role-export run. The old binary key's ID and name
were uninitialized, so no deterministic original binary fixture is claimed.
Tests instead freeze its 541-byte packed layout and independently verify every
initialized key byte, including ID/name offsets, termination and 128 variables.
