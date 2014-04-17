CREATE TABLE jobcards__history
(
    rowid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    jobcard_id INTEGER NOT NULL,
    ctime REAL NOT NULL DEFAULT CURRENT_TIMESTAMP,
    data1 TEXT,
    data2 TEXT
);

CREATE TABLE jobcards__index
(
    jobcard_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    deleted INTEGER(1) NOT NULL DEFAULT 0,
    first_rowid INTEGER NOT NULL DEFAULT 0,
    last_rowid INTEGER NOT NULL DEFAULT 0
);

CREATE VIEW jobcards AS
    SELECT jci.jobcard_id, jch.ctime, jch.data1, jch.data2
    FROM jobcards__index jci, jobcards__history jch
    WHERE jch.rowid = jci.last_rowid
        AND jci.deleted = 0;


CREATE TRIGGER jobcards__ii_update_index INSTEAD OF INSERT ON jobcards
BEGIN
    SELECT
            CASE
                WHEN NEW.jobcard_id IS NOT NULL AND MIN(rowid) IS NOT NULL
                    THEN RAISE(ABORT, 'duplicate key')
            END
        FROM jobcards__index
        WHERE jobcard_id = NEW.jobcard_id;
    INSERT INTO jobcards__index (jobcard_id) VALUES (NEW.jobcard_id);
    INSERT INTO jobcards__history
        (jobcard_id, data1, data2)
        VALUES (
            IFNULL(NEW.jobcard_id, last_insert_rowid()),
            NEW.data1,
            NEW.data2
        );
    UPDATE jobcards__index
        SET first_rowid = last_insert_rowid(),
            last_rowid = last_insert_rowid()
        WHERE jobcard_id = (
                SELECT jobcard_id
                FROM jobcards__history
                WHERE rowid = last_insert_rowid()
            );
END;

