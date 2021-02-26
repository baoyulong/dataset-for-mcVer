// Auction_0x91d0013742c6a6a033d46ac9da7b5e0416c35e24
pragma solidity ^0.4.11;
contract AuctionItem {
    address public owner; 
    bool auctionEnded = false;
    
    uint public currentHighestBid = 0;
    address public highBidder; 

    modifier onlyOwner {
        require(msg.sender == owner);
        _;
    }
    modifier higherBid {
        require(msg.value > currentHighestBid);
        _;
    }
    
    modifier auctionNotOver {
        require(auctionEnded == false);
        _;
    }
    
    constructor(uint startingBid) {
        owner = msg.sender;
        currentHighestBid = startingBid;
    }

    function bid() payable{
        require(msg.value > currentHighestBid);
        require(auctionEnded == false);
        if(highBidder!=0){
            require(highBidder.transfer(currentHighestBid));
        }
        currentHighestBid = msg.value;
        highBidder = msg.sender;
    }

    function endAuction() onlyOwner{
        selfdestruct(owner);
        auctionEnded = true;
    }

    
}