// ============================================================================
// FacturationPro Enterprise — DBConnection.h
// Singleton database connection manager using FireDAC + SQLite
//
// This is a CODE SNIPPET — partial extract for demonstration purposes.
// The full source is proprietary and not distributed.
// ============================================================================

#ifndef DBConnectionH
#define DBConnectionH

#include <System.hpp>
#include <FireDAC.Comp.Client.hpp>
#include <FireDAC.Phys.SQLite.hpp>

/**
 * DBConnection — Singleton wrapper around FireDAC's TFDConnection.
 *
 * Design decisions:
 *   - Singleton: one connection for the entire app lifetime.
 *   - Lazy init: connection is only created on first access.
 *   - Pooling: FireDAC connection pool enabled (max 50 items).
 *   - Foreign keys: enforced via PRAGMA on connect.
 */
class DBConnection {
private:
    static DBConnection* instance;
    TFDConnection* connection;
    String databasePath;

    // Private constructor — prevents direct instantiation
    DBConnection() : connection(NULL) {
        databasePath = ExtractFilePath(ParamStr(0)) + "Database\\facturation.db";
    }

    // Non-copyable
    DBConnection(const DBConnection&);
    DBConnection& operator=(const DBConnection&);

public:
    /** Get the singleton instance */
    static DBConnection* getInstance() {
        if (!instance) {
            instance = new DBConnection();
        }
        return instance;
    }

    /** Initialize the FireDAC connection with SQLite driver */
    void initialize() {
        if (connection != NULL) return;  // Already initialized

        try {
            connection = new TFDConnection(NULL);
            connection->DriverName = "SQLite";

            // Ensure the database directory exists
            String dbDir = ExtractFilePath(databasePath);
            if (!DirectoryExists(dbDir)) {
                ForceDirectories(dbDir);
            }

            // FireDAC connection parameters
            connection->Params->Clear();
            connection->Params->Values["Database"]   = databasePath;
            connection->Params->Values["DriverID"]   = "SQLite";
            connection->Params->Values["LockingMode"]  = "Normal";
            connection->Params->Values["Synchronous"]  = "Normal";
            connection->Params->Values["ForeignKeys"]  = "True";  // FK enforcement

            // Connection pooling
            connection->Params->Values["Pooled"]              = "True";
            connection->Params->Values["POOL_MaximumItems"]   = "50";

            connection->Connected = true;

        } catch (Exception& e) {
            throw Exception("Failed to initialize database: " + e.Message);
        }
    }

    /** Get the active TFDConnection (initializes if needed) */
    TFDConnection* getConnection() {
        if (!connection) initialize();
        return connection;
    }

    /** Execute a non-SELECT SQL statement */
    void execSQL(const String& sql) {
        getConnection()->ExecSQL(sql);
    }

    /** Create a new TFDQuery bound to this connection */
    TFDQuery* createQuery() {
        TFDQuery* query = new TFDQuery(NULL);
        query->Connection = getConnection();
        return query;
    }

    // --- Transaction support ---
    void beginTransaction() { getConnection()->StartTransaction(); }
    void commit()           { getConnection()->Commit(); }
    void rollback()         { getConnection()->Rollback(); }

    /** Check if the database file exists on disk */
    bool databaseExists() { return FileExists(databasePath); }

    /** Get the full path to the database file */
    String getDatabasePath() const { return databasePath; }

    /** Close the connection */
    void close() {
        if (connection && connection->Connected)
            connection->Connected = false;
    }

    ~DBConnection() {
        close();
        if (connection) { delete connection; connection = NULL; }
    }
};

// Static instance initialization
DBConnection* DBConnection::instance = NULL;

// ... (additional connection health-check and reconnect logic not shown)

#endif
