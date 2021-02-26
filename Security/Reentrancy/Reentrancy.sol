pragma solidity ^0.4.19;
contract Victim{
    mapping(address => uint) public userBalance;
    uint public amount=0;
    function withDraw(){
        amount = userBalance[msg.sender];
        if(amount>0){
            msg.sender.call.value(amount)();
            userBalance[msg.sender] = 0;
        }
    }
    fuction receiveEther() payable{
        if(msg.value>0){
            userBalance[msg.sender] += msg.value;
        }
    }
}