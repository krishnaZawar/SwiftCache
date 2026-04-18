WAL test-flow documentation (reduced suite)

Scope:
    This WAL suite is intentionally trimmed to 20 total flows.

Split:
    - Persistence flows: 12 (flow 1 to flow 12)
    - Compaction flows: 8 (flow 13 to flow 20)

Scenarios:
    - 4 persistence scenarios
    - 4 compaction scenarios

Files:
    1) wal-persistence-flows.txt
    2) wal-compaction-flows.txt

Implementation source:
    - test/test_wal.cpp
    - entrypoint: sanityCheck_walPersistence()
