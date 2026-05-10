// ============================================================================
// FacturationPro Enterprise — ClientDAO.h
// Data Access Object for Client entities, using the BaseDAO<T> generic pattern.
//
// This is a CODE SNIPPET — partial extract for demonstration purposes.
// The full source is proprietary and not distributed.
// ============================================================================

#ifndef ClientDAOH
#define ClientDAOH

#include "BaseDAO.h"
#include "../Models/Client.h"

// Typedef to avoid namespace collision with FireDAC's own Client class
typedef ::Client ClientEntity;

/**
 * ClientDAO — Handles all database operations for Client records.
 *
 * Inherits from BaseDAO<ClientEntity> which provides:
 *   - Generic query execution and error handling
 *   - The contract: mapRow(), setInsertParams(), setUpdateParams()
 *
 * This class just implements those three methods + any client-specific queries.
 */
class ClientDAO : public BaseDAO<ClientEntity> {
protected:
    /**
     * mapRow — Convert a database row into a ClientEntity object.
     * Called by BaseDAO after every SELECT query.
     */
    ClientEntity* mapRow(TFDQuery* query) {
        ClientEntity* client = new ClientEntity();

        client->setId(query->FieldByName("Id")->AsInteger);
        client->setName(query->FieldByName("Name")->AsString);
        client->setEmail(query->FieldByName("Email")->AsString);
        client->setPhone(query->FieldByName("Phone")->AsString);
        client->setAddress(query->FieldByName("Address")->AsString);
        client->setCity(query->FieldByName("City")->AsString);
        client->setPostalCode(query->FieldByName("PostalCode")->AsString);
        client->setTaxId(query->FieldByName("TaxId")->AsString);
        client->setCreditLimit(query->FieldByName("CreditLimit")->AsFloat);
        client->setTotalSpent(query->FieldByName("TotalSpent")->AsFloat);
        client->setOutstandingBalance(query->FieldByName("OutstandingBalance")->AsFloat);
        client->setNotes(query->FieldByName("Notes")->AsString);

        return client;
    }

    /** Bind parameters for an INSERT query */
    void setInsertParams(TFDQuery* query, const ClientEntity& client) {
        query->ParamByName("name")->AsString          = client.getName();
        query->ParamByName("email")->AsString         = client.getEmail();
        query->ParamByName("phone")->AsString         = client.getPhone();
        query->ParamByName("address")->AsString       = client.getAddress();
        query->ParamByName("city")->AsString          = client.getCity();
        query->ParamByName("postal_code")->AsString   = client.getPostalCode();
        query->ParamByName("tax_id")->AsString        = client.getTaxId();
        query->ParamByName("credit_limit")->AsFloat   = client.getCreditLimit();
        query->ParamByName("notes")->AsString         = client.getNotes();
    }

    /** Bind parameters for an UPDATE query (includes computed fields) */
    void setUpdateParams(TFDQuery* query, const ClientEntity& client) {
        setInsertParams(query, client);  // Reuse insert params as base
        // UPDATE also sets these computed fields:
        query->ParamByName("total_spent")->AsFloat     = client.getTotalSpent();
        query->ParamByName("outstanding")->AsFloat     = client.getOutstandingBalance();
        query->ParamByName("id")->AsInteger            = client.getId();
    }

public:
    ClientDAO() : BaseDAO<ClientEntity>("Client") {}

    /**
     * insert — Save a new client to the database.
     * Auto-generates the client code (CLT001, CLT002...) if not set.
     */
    bool insert(ClientEntity& client) {
        TFDQuery* query = db->createQuery();
        try {
            // Auto-generate code if empty
            if (client.getCode().IsEmpty()) {
                client.setCode(getNextCode());
            }

            query->SQL->Text =
                "INSERT INTO Clients (Code, Name, Email, Phone, Address, City, "
                "PostalCode, TaxId, CreditLimit, Notes) "
                "VALUES (:code, :name, :email, :phone, :address, :city, "
                ":postal_code, :tax_id, :credit_limit, :notes)";

            query->ParamByName("code")->AsString = client.getCode();
            setInsertParams(query, client);
            query->ExecSQL();

            // Retrieve the auto-generated ID
            client.setId(query->Connection->GetLastInsertRowID());

            delete query;
            return true;
        } catch (Exception& e) {
            delete query;
            throw;  // Let the caller handle the error
        }
    }

    // ... (update, delete, findById, search methods not shown)
    // ... (getNextCode — generates sequential CLTnnn codes — not shown)

private:
    /**
     * getNextCode — Finds the highest existing CLT code and increments.
     * Example: if CLT017 exists, returns "CLT018"
     */
    String getNextCode();
    // ... (implementation not shown)
};

#endif
