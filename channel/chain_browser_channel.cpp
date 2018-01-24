﻿/*Copyright 2017 hyperchain.net  (Hyper Block Chain)
/*
/*Distributed under the MIT software license, see the accompanying
/*file COPYING or https://opensource.org/licenses/MIT.
/*
/*Permission is hereby granted, free of charge, to any person obtaining a copy of this 
/*software and associated documentation files (the "Software"), to deal in the Software
/*without restriction, including without limitation the rights to use, copy, modify, merge,
/*publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
/*to whom the Software is furnished to do so, subject to the following conditions:
/*
/*The above copyright notice and this permission notice shall be included in all copies or
/*substantial portions of the Software.
/*
/*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
/*INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
/*PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
/*FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
/*OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/*DEALINGS IN THE SOFTWARE.
*/

#include "chain_browser_channel.h"
#include "HChainP2PManager/interface/QtInterface.h"
#include "wnd/block_detail_dlg.h"
#include "util/commonutil.h"

#include <QDateTime>
#include <QDebug>

chain_browser_channel::chain_browser_channel(QObject *parent) : QObject(parent)
{
    detailDlg_ = QSharedPointer<block_detail_dlg>(new block_detail_dlg);
}

void chain_browser_channel::onHtmlReady()
{
    QVariantMap m;
    std::string dataSize;
    GetChainData(dataSize);
    m["chainDataSize"] = QString::fromStdString(dataSize);
	m["tradesTotal"] = GetPoeReqTotalNum();
	m["nodeCount"] = GetAllConnectedNodes();
	m["unConfirmedReq"] = GetAllConfirmingPoeReqNum();
	m["myConnectedNode"] = GetConnNodesNum();

	m["alternativeConsensusBlockTime"] = GetStartTimeOfCurrentConsensus();

    emit sigUpdateGeneralInfo(m);
}

void chain_browser_channel::search(QString key)
{
    std::string strKey = key.toStdString();
	VEC_T_BROWSERSHOWINFO vec = Query(strKey);

    QList<QVariant> li;

    if(vec.empty()){
        emit sigSearchResult(QVariant(li));
        return;
    }

    for(auto item : vec){
        QVariantMap m;

		m["iBlockNum"] = item.iBlockNo;
		m["iUseNodes"] = item.iJoinedNodeNum;
		m["iDataChainNum"] = item.iLocalBlockNum;
		m["iChainNum"] = item.iLocalChainNum;
		m["tTime"] = item.tTimeStamp;

        QVariantMap mf;
		convertEvidenceStruct2VariantMap(mf, &item.tPoeRecordInfo);
        m["tFileInfo"] = mf;

        li << m;
    }

    emit sigSearchResult(QVariant(li));
}

void chain_browser_channel::showDetail(QVariantMap generalInfo)
{

    detailDlg_->updateDetail(generalInfo);

    detailDlg_->show(true);
}

int chain_browser_channel::getConfirmingNum()
{
	return GetAllConfirmingPoeReqNum();
}

void chain_browser_channel::getLastBlockInfo()
{
    uint64 blockNum = GetCurBlockNumOfAllNode();
    QString blockNumStr = QString::number(blockNum);

    search(blockNumStr);
}

void chain_browser_channel::setLanguage(int lang)
{
    emit sigChangeLang(lang);

    detailDlg_->setLanguage(lang);
}
