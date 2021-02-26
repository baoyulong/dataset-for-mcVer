// Auction_0x64Cc1a7DFE15F69b2B5Dbe80B4e40a51aaA7917c
pragma solidity ^0.4.21;
contract Auction {
    uint duration = 5;
    address owner;
    uint public prize;

    uint public bids;
    address public leader;
    uint public deadline;
    bool public claimed;

    constructor(uint block_timestamp) public payable {
        owner = msg.sender;
        prize = msg.value;
        bids = 0;
        leader = msg.sender;
        deadline = block_timestamp + duration;
        claimed = false;
    }

    function bid(uint block_timestamp) public payable {
        require(block_timestamp <= deadline);
        require(msg.value == bids+4);
        owner.transfer(msg.value);
        bids++;
        leader = msg.sender;
        deadline = block_timestamp + duration;
    }
}
