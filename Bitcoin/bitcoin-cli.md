# bitcoin-cli 命令

A、一般性的命令

- help ( "command" )
- stop
- getinfo
- ping
- getnettotals
- getnetworkinfo
- getpeerinfo
- getconnectioncount
- verifychain ( checklevel numblocks )
- getaddednodeinfo dns ( "node" )
- addnode "node" "add|remove|onetry"


B、钱包、账户、地址、转帐、发消息

- getwalletinfo
- walletpassphrase "passphrase" timeout
- walletlock
- walletpassphrasechange "oldpassphrase" "newpassphrase"
- backupwallet "destination"
- importwallet "filename"
- dumpwallet "filename"

- listaccounts ( minconf )
- getaddressesbyaccount "account"
- getaccountaddress "account"
- getaccount "bitcoinaddress"
- validateaddress "bitcoinaddress"
- dumpprivkey "bitcoinaddress"
- setaccount "bitcoinaddress" "account"
- getnewaddress ( "account" )
- keypoolrefill ( newsize )
- importprivkey "bitcoinprivkey" ( "label" rescan )
- createmultisig nrequired ["key",...]
- addmultisigaddress nrequired ["key",...] ( "account" )

- getbalance ( "account" minconf )
- getunconfirmedbalance
- getreceivedbyaccount "account" ( minconf )
- listreceivedbyaccount ( minconf includeempty )
- getreceivedbyaddress "bitcoinaddress" ( minconf )
- listreceivedbyaddress ( minconf includeempty )
- move "fromaccount" "toaccount" amount ( minconf "comment" )
- listunspent ( minconf maxconf  ["address",...] )
- listlockunspent
- lockunspent unlock [{"txid":"txid","vout":n},...]

- getrawchangeaddress
- listaddressgroupings
- settxfee amount
- sendtoaddress "bitcoinaddress" amount ( "comment" "comment-to" )
- sendfrom "fromaccount" "tobitcoinaddress" amount ( minconf "comment" "comment-to" )
- sendmany "fromaccount" {"address":amount,...} ( minconf "comment" )

- signmessage "bitcoinaddress" "message"
- verifymessage "bitcoinaddress" "signature" "message"


C、Tx、Block、Ming

- createrawtransaction [{"txid":"id","vout":n},...] {"address":amount,...}
- signrawtransaction "hexstring" ( [{"txid":"id","vout":n,"scriptPubKey":"hex","redeemScript":"hex"},...] ["privatekey1",...] sighashtype )
- sendrawtransaction "hexstring" ( allowhighfees )


- gettransaction "txid"
- listtransactions ( "account" count from )
- listsinceblock ( "blockhash" target-confirmations )


- getrawmempool ( verbose )
- gettxoutsetinfo
- gettxout "txid" n ( includemempool )
- getrawtransaction "txid" ( verbose )
- decoderawtransaction "hexstring"
- decodescript "hex"

- getblockchaininfo
- getblockcount
- getbestblockhash
- getblockhash index
- getblock "hash" ( verbose )

- getmininginfo
- getdifficulty
- getnetworkhashps ( blocks height )
- gethashespersec                                                                                                                                                       
- getgenerate
- setgenerate generate ( genproclimit )
- getwork ( "data" )
- getblocktemplate ( "jsonrequestobject" )
- submitblock "hexdata" ( "jsonparametersobject" )
