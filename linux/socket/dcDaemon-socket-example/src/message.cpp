#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "commonInterface.h"
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <json/json.h>
#include <stdlib.h> // exit()
#include <string.h> // memset()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_TCP, htons(), htons()
#include <arpa/inet.h>  // inet_addr()
#include "msgThread.h"
#include "tcpServer.h"
#include "tcpSocket.h"
#include "config.h"
#include "main.h"
#include "message.h"
#include "utility.h"

using namespace std;

Serializer& MsgHeader::operator<<(Serializer& out) { m_senderID = _threadID(); out << m_msgID << m_msgBodyLen << m_senderID; return out; }
Serializer& MsgHeader::operator>>(Serializer& in) { in >> m_msgID >> m_msgBodyLen >> m_senderID; return in; }

Serializer& CommonMessage::operator<<(Serializer& out) { out << m_msgHeader; return out; }
Serializer& CommonMessage::operator>>(Serializer& in) { in >> m_msgHeader; return in; }

Serializer& TestMessage::operator<<(Serializer& out) { CommonMessage::operator<<(out); out << m_testID; return out; }
Serializer& TestMessage::operator>>(Serializer& in) {  CommonMessage::operator>>(in); in >> m_testID; return in; }

Serializer& AcceptSocketMessage::operator<<(Serializer& out) { CommonMessage::operator<<(out); out << m_channelID; return out; }
Serializer& AcceptSocketMessage::operator>>(Serializer& in) {  CommonMessage::operator>>(in); in >> m_channelID; return in; }

Serializer& CloseSocketMessage::operator<<(Serializer& out) { CommonMessage::operator<<(out); out << m_channelID; return out; }
Serializer& CloseSocketMessage::operator>>(Serializer& in) {  CommonMessage::operator>>(in); in >> m_channelID; return in; }

Serializer& ChannelMessage::operator<<(Serializer& out) { CommonMessage::operator<<(out); out << m_channelID << m_recvByteArray; return out; }
Serializer& ChannelMessage::operator>>(Serializer& in) {  CommonMessage::operator>>(in); in >> m_channelID >> m_recvByteArray; return in; }

#if 0


AcceptSocketMessageType* allocateAcceptSocketMessage(int socket)
{
  AcceptSocketMessageType *pMsg = new AcceptSocketMessageType(socket);
  return pMsg;
}

CloseSocketMessageType* allocateCloseSocketMessage(int socket)
{
  CloseSocketMessageType *pMsg = new CloseSocketMessageType(socket);
  return pMsg;  
}

ReceivedMessageType * allocateReceivedMessage(int socket, ByteArray& byteArray)
{
  UINT8 *pucData = new UINT8[sizeof(AcceptSocketMessageType) + 2 * sizeof(int) + byteArray.size()];
  ReceivedMessageType *pMsg = (ReceivedMessageType *)pucData;
  pMsg->init(socket, byteArray);
  return pMsg;
}
#endif

Serializer& operator<<(Serializer& out, MsgHeader& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, MsgHeader& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, CommonMessage& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, CommonMessage& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, TestMessage& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, TestMessage& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, AcceptSocketMessage& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, AcceptSocketMessage& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, CloseSocketMessage& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, CloseSocketMessage& value) {  return value.operator>>(in); }
Serializer& operator<<(Serializer& out, ChannelMessage& value) {  return value.operator<<(out); }
Serializer& operator>>(Serializer& in, ChannelMessage& value) {  return value.operator>>(in); }
