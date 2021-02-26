pragma solidity ^0.4.22;
contract Auction{
    
    address public beneficiary;
    address public highestBidder;
    uint public highestBid;
    bool ended;

    constructor(address _beneficiary) public {
        beneficiary = _beneficiary;
    }
    function bid() public payable{
        require(!ended);
        require(msg.value > highestBid);
        if (highestBidder!=0 ) {
            require(highestBidder.send(highestBid));
        }
        highestBidder = msg.sender;
        highestBid = msg.value;
    }
    function auctionEnd() public{
        require(!ended);
        ended = true;
        beneficiary.transfer(highestBid);
    }
}
