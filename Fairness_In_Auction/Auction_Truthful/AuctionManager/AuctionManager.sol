// Auction_0x37512eec5e3ee7843a1ab59ef99fb52589037774
pragma solidity ^0.4.21;
contract Rewrite{
    bool ended;
    uint256 percentageBidIncrease;
    uint256 auctionEnd;
    uint256 auctionStart;
    uint256 auctionsStartBid;
    uint256 highestBid;
    address previousHighestBidder;
    address highestBidder;

    constructor (address _seller,unit256 _auctionEnd, uint block_timestamp){
        auctionStart = block_timestamp;
        auctionsStartBid = msg.value;
        auctionEnd = _auctionEnd;
        highestBidder = _seller;
        highestBid = 0;
        ended = false;
    }

    function bid(uint block_timestamp) public payable {
        require(ended == false);
        require(auctionEnd >= block_timestamp);
        uint claimBidPrice;
        if(highestBid == 0){
            claimBidPrice =  auctionsStartBid;
        } 
        if (highestBid != 0) {
            uint256 requiredBid = highestBid +2;
            claimBidPrice = requiredBid;
        }
        uint256 bidValue = msg.value;
        require(bidValue >= claimBidPrice);
        previousHighestBidder = highestBidder;
        highestBid = msg.value;
        highestBidder = msg.sender;
    }

    function endAuction(){
        require(ended == false);
        require(auctionEnd < now);
        ended = true;
    }
}