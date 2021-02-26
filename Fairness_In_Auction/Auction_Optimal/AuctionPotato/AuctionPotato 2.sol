// Auction_0x433b189d5fbdfee89e3a9f4c6b9469495fcb00f1
pragma solidity ^0.4.21;

contract AuctionPotato {
    // static
    address public owner;
    uint public startTime;
    uint public endTime;
    
    // start auction manually at given time
    bool started;

    // pototo
    uint public potato;
    uint oldPotato;
    uint oldHighestBindingBid;
    
    // transfer ownership
    address creatureOwner;
    address creature_newOwner;
    
    // state
    bool public canceled;
    
    uint public highestBindingBid;
    address public highestBidder;
    
    // used to immidiately block placeBids
    bool blockerPay;
    bool blockerWithdraw;
    
    mapping(address => uint256) public fundsByBidder;
    bool ownerHasWithdrawn;
    
    
    // initial settings on contract creation
    constructor() public {
        blockerWithdraw = false;
        blockerPay = false;
        owner = msg.sender;
        creatureOwner = owner;
        highestBindingBid = 1;
        potato = 0;
        started = false;
    }
    
    // command to start the auction
    function startAuction(uint block_timestamp,uint _duration_secs) public {
        require(msg.sender == owner);
        require(started == false);
        started = true;
        startTime = block_timestamp;
        endTime = block_timestamp + _duration_secs;     
    }

    function placeBid(uint block_timestamp) public
        payable
    {
        require(block_timestamp >= startTime);
        require(block_timestamp <= endTime);
        require(!canceled);
        require(msg.sender != owner);
        require(msg.value == highestBindingBid+potato);
        require(msg.sender != highestBidder);
        require(started == true);
        require(blockerPay == false);
        blockerPay = true;

        fundsByBidder[msg.sender] = fundsByBidder[msg.sender]+highestBindingBid;
        fundsByBidder[highestBidder] = fundsByBidder[highestBidder]+potato;
        
        oldHighestBindingBid = highestBindingBid;
        
        highestBidder = msg.sender;
        highestBindingBid = highestBindingBid+potato;
        oldPotato = potato;
        potato = highestBindingBid - 2;
        blockerPay = false;
    }

    function cancelAuction() public
        onlyOwner
        onlyBeforeEnd
        onlyNotCanceled
    {
        canceled = true;
    }

    modifier onlyOwner {
        require(msg.sender == owner);
        _;
    }

    modifier onlyNotOwner {
        require(msg.sender != owner);
        _;
    }

    modifier onlyAfterStart {
        require(block_timestamp < startTime);
        _;
    }

    modifier onlyBeforeEnd {
        require(block_timestamp <= endTime);
        _;
    }

    modifier onlyNotCanceled {
        require(!canceled);
        _;
    }
    
    // who owns the creature (not necessarily auction winner)
    function queryCreatureOwner() public view returns (address _creatureOwner) {
        return creatureOwner;
    }
    
    // transfer ownership for auction winners in case they want to trade the creature before release
    function transferCreatureOwnership(address _newOwner) public {
        require(msg.sender == creatureOwner);
        creature_newOwner = _newOwner;
    }
    
    // buyer needs to confirm the transfer
    function acceptCreatureOwnership() public {
        require(msg.sender == creature_newOwner);
        creatureOwner = creature_newOwner;
        creature_newOwner = 0;
    }
    
}