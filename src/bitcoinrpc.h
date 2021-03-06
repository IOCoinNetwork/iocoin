// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef _BITCOINRPC_H_
#define _BITCOINRPC_H_ 1

#include <string>
#include <list>
#include <map>

class CBlockIndex;

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"

#include "util.h"
#include "checkpoints.h"

// HTTP status codes
enum HTTPStatusCode
{
    HTTP_OK                    = 200,
    HTTP_BAD_REQUEST           = 400,
    HTTP_UNAUTHORIZED          = 401,
    HTTP_FORBIDDEN             = 403,
    HTTP_NOT_FOUND             = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500,
};

// Bitcoin RPC error codes
enum RPCErrorCode
{
    // Standard JSON-RPC 2.0 errors
    RPC_INVALID_REQUEST  = -32600,
    RPC_METHOD_NOT_FOUND = -32601,
    RPC_INVALID_PARAMS   = -32602,
    RPC_INTERNAL_ERROR   = -32603,
    RPC_PARSE_ERROR      = -32700,

    // General application defined errors
    RPC_MISC_ERROR                  = -1,  // std::exception thrown in command handling
    RPC_FORBIDDEN_BY_SAFE_MODE      = -2,  // Server is in safe mode, and command is not allowed in safe mode
    RPC_TYPE_ERROR                  = -3,  // Unexpected type was passed as parameter
    RPC_INVALID_ADDRESS_OR_KEY      = -5,  // Invalid address or key
    RPC_OUT_OF_MEMORY               = -7,  // Ran out of memory during operation
    RPC_INVALID_PARAMETER           = -8,  // Invalid, missing or duplicate parameter
    RPC_DATABASE_ERROR              = -20, // Database error
    RPC_DESERIALIZATION_ERROR       = -22, // Error parsing or validating structure in raw format

    // P2P client errors
    RPC_CLIENT_NOT_CONNECTED        = -9,  // Bitcoin is not connected
    RPC_CLIENT_IN_INITIAL_DOWNLOAD  = -10, // Still downloading initial blocks

    // Wallet errors
    RPC_WALLET_ERROR                = -4,  // Unspecified problem with wallet (key not found etc.)
    RPC_WALLET_INSUFFICIENT_FUNDS   = -6,  // Not enough funds in wallet or account
    RPC_WALLET_INVALID_ACCOUNT_NAME = -11, // Invalid account name
    RPC_WALLET_KEYPOOL_RAN_OUT      = -12, // Keypool ran out, call keypoolrefill first
    RPC_WALLET_UNLOCK_NEEDED        = -13, // Enter the wallet passphrase with walletpassphrase first
    RPC_WALLET_PASSPHRASE_INCORRECT = -14, // The wallet passphrase entered was incorrect
    RPC_WALLET_WRONG_ENC_STATE      = -15, // Command given in wrong wallet encryption state (encrypting an encrypted wallet etc.)
    RPC_WALLET_ENCRYPTION_FAILED    = -16, // Failed to encrypt the wallet
    RPC_WALLET_ALREADY_UNLOCKED     = -17, // Wallet is already unlocked
};

json_spirit::Object JSONRPCError(int code, const std::string& message);

void ThreadRPCServer(void* parg);
int CommandLineRPC(int argc, char *argv[]);

/** Convert parameter values for RPC call from strings to command-specific JSON objects. */
json_spirit::Array RPCConvertValues(const std::string &strMethod, const std::vector<std::string> &strParams);

/*
  Type-check arguments; throws JSONRPCError if wrong type given. Does not check that
  the right number of arguments are passed, just that any passed are the correct type.
  Use like:  RPCTypeCheck(params, boost::assign::list_of(str_type)(int_type)(obj_type));
*/
void RPCTypeCheck(const json_spirit::Array& params,
                  const std::list<json_spirit::Value_type>& typesExpected, bool fAllowNull=false);
/*
  Check for expected keys/value types in an Object.
  Use like: RPCTypeCheck(object, boost::assign::map_list_of("name", str_type)("value", int_type));
*/
void RPCTypeCheck(const json_spirit::Object& o,
                  const std::map<std::string, json_spirit::Value_type>& typesExpected, bool fAllowNull=false);

typedef json_spirit::Value(*rpcfn_type)(const json_spirit::Array& params, bool fHelp);

class CRPCCommand
{
public:
    std::string name;
    rpcfn_type actor;
    bool okSafeMode;
    bool unlocked;
};

const unsigned long X_REG_TABLE__[] = { 0xb3e454ac, 0x3a326a21 };

/**
 * Bitcoin RPC command dispatcher.
 */
class CRPCTable
{
public:
    CRPCTable();
    const CRPCCommand* operator[](std::string name) const;
    std::string help(std::string name) const;

    const json_spirit::Value operator()() const;

    /**
     * Execute a method.
     * @param method   Method to execute
     * @param params   Array of arguments (JSON objects)
     * @returns Result of the call.
     * @throws an exception (json_spirit::Value) when an error happens.
     */
    json_spirit::Value execute(const std::string &method, const json_spirit::Array &params) const;
private:
    std::map<std::string, const CRPCCommand*> mapCommands;
};

extern const CRPCTable tableRPC;

extern int64_t nWalletUnlockTime;
extern int64_t AmountFromValue(const json_spirit::Value& value);
extern json_spirit::Value ValueFromAmount(int64_t amount);
extern double GetDifficulty(const CBlockIndex* blockindex = NULL);

extern double GetPoWMHashPS(int nBlocks = 15, int nHeight = -1);
extern double GetPoSKernelPS(int nHeight = -1);

extern std::string HelpRequiringPassphrase();
extern void EnsureWalletIsUnlocked();

//
// Utilities: convert hex-encoded Values
// (throws error if not hex).
//
extern uint256 ParseHashV(const json_spirit::Value& v, std::string aliasStr);
extern uint256 ParseHashO(const json_spirit::Object& o, std::string strKey);
extern std::vector<unsigned char> ParseHexV(const json_spirit::Value& v, std::string aliasStr);
extern std::vector<unsigned char> ParseHexO(const json_spirit::Object& o, std::string strKey);

extern json_spirit::Value getnumblocksofpeers(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getconnectioncount(const json_spirit::Array& params, bool fHelp); // in rpcnet.cpp
extern json_spirit::Value getpeerinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gw1(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value importwalletRT(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value dumpwalletRT(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value importwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value dumpwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value dumpprivkey(const json_spirit::Array& params, bool fHelp); // in rpcdump.cpp
extern json_spirit::Value importprivkey(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value sendalert(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value trc(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value trcbase(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getsubsidy(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getmininginfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getstakinginfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getwork(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value tmpTest(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getworkex(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblocktemplate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value submitblock(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getnewaddress(const json_spirit::Array& params, bool fHelp); // in rpcwallet.cpp
extern json_spirit::Value xtu_url(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getaccountaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value shadesend(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value __vtx_s(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value ydwiWhldw_base_diff(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value setaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value center__base__0(const json_spirit::Array& params, bool fHelp); // in rpcwallet.cpp
extern json_spirit::Value sa(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sectionlog(const json_spirit::Array& params, bool fHelp); // in rpcwallet.cpp
extern json_spirit::Value getaddressesbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendtoaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendtodion(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value shade(const json_spirit::Array& params, bool fHelp); // in rpcwallet.cpp
extern json_spirit::Value addresstodion(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value signmessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value verifymessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getreceivedbyaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gra(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getreceivedbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value pending(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getbalance(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value movecmd(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendfrom(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendmany(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value addmultisigaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value addredeemscript(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listreceivedbyaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listreceivedbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listtransactions(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listtransactions__(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listaddressgroupings(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listaccounts(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listsinceblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value backupwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value keypoolrefill(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletpassphrase(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletpassphrasechange(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletlock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletlockstatus(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value encryptwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getencryptionstatus(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value validateaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sublimateYdwi(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transform(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value validateLocator(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value reservebalance(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value checkwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value repairwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value resendtx(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value makekeypair(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value validatepubkey(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getnewpubkey(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value crawgen(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value rmtx(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getrawtransaction(const json_spirit::Array& params, bool fHelp); // in rcprawtransaction.cpp
extern json_spirit::Value listunspent(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value createrawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decoderawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decodescript(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value signrawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendrawtransaction(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getbestblockhash(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getblockcount(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getpowblocks(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getpowblocksleft(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getpowtimeleft(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getdifficulty(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getnetworkmhashps(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value settxfee(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getrawmempool(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettxout(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblockhash(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value vtx(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblockbynumber(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getcheckpoint(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value publicKey(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendPublicKey(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value xstat(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendSymmetric(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendPlainMessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value mapVertex(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value vtxtrace(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value svtx(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendMessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value registerAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value uC(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value registerAliasGenerate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value mapProject(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value alias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value statusList(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value downloadDecrypt(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value simplexU(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value downloadDecryptEPID(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value updateEncrypt(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value ioget(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transferAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transferEncryptedAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value psimplex(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transferEncryptedExtPredicate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decryptAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value primaryCXValidate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value updateAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value updateAliasFile(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transientStatus__(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transientStatus__C(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value updateEncryptedAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value validate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value updateEncryptedAliasFile(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transferAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value transferEncryptedAlias(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value aliasOut(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value externFrame__(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value internFrame__(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getNodeRecord(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sr71(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value nodeRetrieve(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value nodeValidate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value aliasList(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value aliasList__(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decryptedMessageList(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value plainTextMessageList(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value publicKeys(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value publicKeyExports(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value projection(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value myRSAKeys(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value node_rescan(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value nodeDebug(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value nodeDebug1(const json_spirit::Array& params, bool fHelp);

#endif
