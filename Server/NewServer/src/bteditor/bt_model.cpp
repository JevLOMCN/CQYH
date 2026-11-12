#include "stdafx.h"
#include <QtGui>
#include <QSettings>

#include "btnod.h"
#include "bt_manager.h"

#include "undoredo_commands.h"

#include "bt_script.h"

#include "bt_model.h"

#define MAX_COLUMN_COUNT	(5)
static char* s_szHeaderList[MAX_COLUMN_COUNT] = {
	"Action", "Param 0", "Param 1", "Param 2", "Param 3"
};
static QSize s_nSizeHint[MAX_COLUMN_COUNT] = {
	QSize(300, 24), QSize(100, 24), QSize(100, 24), QSize(100, 24), QSize(100, 24)
};

BTModel::BTModel(QUndoStack * undoStack, QObject *parent)
    : QAbstractItemModel(parent)
    , m_pUndoStack(undoStack)
    , m_ResultByCommands(false)
{
	m_pBTreeFile = NULL;
    m_pHoldTree = NULL;
}

BTModel::~BTModel()
{
    UnInit();
}

BOOL BTModel::Init(CBTreeFile* pBTreeFile)
{
	LOG_PROCESS_ERROR(pBTreeFile);

	m_pBTreeFile = pBTreeFile;

	return TRUE;
Exit0:

	return FALSE;
}

BOOL BTModel::UnInit(void)
{
	m_pBTreeFile = NULL;

	return TRUE;
}

BOOL BTModel::ReLoad(const char* pcszFileName)
{
    INT32 nRetCode = FALSE;

    if(m_pUndoStack)
    {
        m_pUndoStack->clear();
    }

    reset(); // reset view

    Q_ASSERT(m_pBTreeFile);

    nRetCode = m_pBTreeFile->UnInit();
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pBTreeFile->Init();
    LOG_PROCESS_ERROR(nRetCode);

    if ( pcszFileName )
    {
        nRetCode = m_pBTreeFile->Load(pcszFileName);
        LOG_PROCESS_ERROR(nRetCode);
    }

Exit0:
    return nRetCode;
}

CBTreeFile* BTModel::GetBTreeFile(void)
{
	return m_pBTreeFile;
}

bool BTModel::SetFileName(const char* pcszFileName)
{
    m_pBTreeFile->SetFileName( pcszFileName );
    m_pBTreeFile->SetModified(); // to save

    return false;
}

int BTModel::columnCount(const QModelIndex &parent) const
{
	return MAX_COLUMN_COUNT;
}

QVariant BTModel::data(const QModelIndex &index, int role) const
{
	QVariant Result;

    PROCESS_ERROR(index.isValid());
	PROCESS_ERROR(index.column() < MAX_COLUMN_COUNT);
    if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
		LOG_PROCESS_ERROR(pNode);

		if (index.column() == 0)
			Result = QString(pNode->szType);
		else if (index.column() < MAX_COLUMN_COUNT)
			Result = QString(pNode->szParam[index.column() - 1]);
	}
	else if (role == Qt::SizeHintRole)
	{
		return s_nSizeHint[index.column()];
	}

Exit0:
	return Result;
}

QVariant BTModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
	PROCESS_ERROR(orientation == Qt::Horizontal);
	if (role == Qt::DisplayRole && section < MAX_COLUMN_COUNT)
		return QString(s_szHeaderList[section]);
	else if (role == Qt::SizeHintRole)
		return s_nSizeHint[section];
Exit0:
    return QVariant();
}

QModelIndex BTModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex Result;

    PROCESS_ERROR(hasIndex(row, column, parent));

    if (!parent.isValid())
	{
		LOG_PROCESS_ERROR(row < m_pBTreeFile->GetTreeCount());
		Result = createIndex(row, column, m_pBTreeFile->GetRootNode(row));
	}
    else
	{
        BT_EDIT_NOD* pParent = static_cast<BT_EDIT_NOD*>(parent.internalPointer());
		LOG_PROCESS_ERROR(pParent);
		LOG_PROCESS_ERROR(row < pParent->Children.size());

		INT32 nCounter = row;
		std::list<BT_EDIT_NOD*>::iterator it = pParent->Children.begin();
		while (it != pParent->Children.end() && nCounter > 0)
		{
			++it;
			nCounter--;
		}
		LOG_PROCESS_ERROR(it != pParent->Children.end());

		Result = createIndex(row, column, *it);
	}

Exit0:
	return Result;
}

QModelIndex BTModel::parent(const QModelIndex &index) const
{
    QModelIndex currentIndex = QModelIndex();
    LOG_PROCESS_ERROR(index.isValid()); 

    BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
    LOG_PROCESS_ERROR(pNode);
    PROCESS_ERROR(pNode->pParent);
    
    // check exist in this model
    //currentIndex = Nod2Index(pNode);
    //LOG_PROCESS_ERROR(currentIndex.isValid());
    currentIndex = Nod2Index(pNode->pParent);
    return currentIndex;

    //INT32 row = 0;	
    //if (pNode->pParent->pParent)
    //{
    //    std::list<BT_EDIT_NOD*>::iterator it = pNode->pParent->pParent->Children.begin();
    //    while (it != pNode->pParent->pParent->Children.end() && *it != pNode->pParent)
    //    {
    //        ++it;
    //        row++;
    //    }
    //    LOG_PROCESS_ERROR(it != pNode->pParent->pParent->Children.end());
    //}
    //return createIndex(row, 0, pNode->pParent);

Exit0:
	return QModelIndex();
}

int BTModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
	{
		return m_pBTreeFile->GetTreeCount();
	}
    else
	{
        BT_EDIT_NOD* pParent = static_cast<BT_EDIT_NOD*>(parent.internalPointer());
		LOG_PROCESS_ERROR(pParent);

		return pParent->Children.size();
	}

Exit0:
	return 0;
}

Qt::ItemFlags BTModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool BTModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QString oldValue = data(index, Qt::EditRole).toString();
    QString newValue = value.toString();

    BT_EDIT_NOD* pNod = (BT_EDIT_NOD*)index.internalPointer();
    Q_ASSERT(pNod);
    int nNodIndex = GetBTreeFile()->GetNodeIndex(pNod);

    ChangeValueCommand *changeCommand = new ChangeValueCommand( oldValue, newValue, nNodIndex, index.column(), this );
    Q_ASSERT(changeCommand);
    m_pUndoStack->push(changeCommand);   // execute redo, and set result to m_ResultByCommands

    emitModelDirty();
    return m_ResultByCommands;
}

bool BTModel::setDataByCommands(const QModelIndex &index, const QVariant &value, int role)
{
    m_ResultByCommands = false;
    bool bResult = false;

	if (role == Qt::EditRole)
	{
		LOG_PROCESS_ERROR(index.column() < MAX_COLUMN_COUNT);

		BT_EDIT_NOD* pNode = (BT_EDIT_NOD*)index.internalPointer();
		LOG_PROCESS_ERROR(pNode);

		if (index.column() == 0)
			strxcpy(pNode->szType, value.toString().toAscii(), MAX_BT_STRING_LEN);
		else if (index.column() < MAX_COLUMN_COUNT)
			strxcpy(pNode->szParam[index.column() - 1], value.toString().toAscii(), MAX_BT_STRING_LEN);

		m_pBTreeFile->SetModified();
		emit dataChanged(index, index);
	}

    m_ResultByCommands = true;
	return true;
Exit0:
    m_ResultByCommands = false;
	return false;
}

bool BTModel::setHeaderData(int section, Qt::Orientation orientation,
                            const QVariant &value, int role)
{
    return false;
}

bool BTModel::insertSibling( const QModelIndex& current )
{
    int nParentNodIndex = -1;
    BT_EDIT_NOD *pParentNod = (BT_EDIT_NOD *)current.parent().internalPointer();
    if( pParentNod )
    {
        nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNod);
        Q_ASSERT( nParentNodIndex >= 0 );
    }

    InsertCommand *pCmd = new InsertCommand( NULL, nParentNodIndex, current.row()+1, 1, this );
    Q_ASSERT(pCmd);
    m_pUndoStack->push(pCmd);    // execute redo, and set result to m_ResultByCommands

    QModelIndex ParentIndex = QModelIndex();
    if ( nParentNodIndex >= 0)
    {
        ParentIndex = Nod2Index(pParentNod);
    }
    QModelIndex focusIndex = index(current.row()+1, 0, ParentIndex );
    emitItemFocused(focusIndex);
    emitModelDirty();

    return m_ResultByCommands;
}

bool BTModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    return false;
}

bool BTModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    int nParentNodIndex = -1;
    if (parent.isValid())
    {
        BT_EDIT_NOD* pParentNod = (BT_EDIT_NOD*)parent.internalPointer();
        Q_ASSERT(pParentNod);
        nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNod);
    }

    InsertCommand *pCmd = new InsertCommand( NULL, nParentNodIndex, 0, 1, this );
    Q_ASSERT(pCmd);
    m_pUndoStack->push(pCmd);   // execute redo, and set result to m_ResultByCommands

    QModelIndex focusIndex = index(position, 0, parent );
    emitItemFocused(focusIndex);
    emitModelDirty();
    return m_ResultByCommands;
}

bool BTModel::insertRowsByCommands(int position, int rows, const QModelIndex &parent)
{
    Q_ASSERT( rows == 1 );  // only allow select one item 
    m_ResultByCommands = false;

	bool bResult = false;
	std::list<BT_EDIT_NOD*>::iterator it;

	if (parent.isValid())
	{
		BT_EDIT_NOD* pParent = (BT_EDIT_NOD*)parent.internalPointer();
        LOG_PROCESS_ERROR(pParent);

		it = pParent->Children.begin();
		INT32 nCounter = position;
		while (nCounter > 0)
		{
			LOG_PROCESS_ERROR(it != pParent->Children.end());
			nCounter--;
			++it;
		}

		beginInsertRows(parent, position, position + rows - 1);
		for (INT32 nIndex = 0; nIndex < rows; nIndex++)
		{
			BT_EDIT_NOD* pNode = new BT_EDIT_NOD;
			LOG_PROCESS_ERROR(pNode);

			pNode->pParent = pParent;
			strxcpy(pNode->szType, "batInvalid", MAX_BT_STRING_LEN);
			pNode->szParam[0][0] = pNode->szParam[1][0] = pNode->szParam[2][0] = pNode->szParam[3][0] = 0;
            pParent->Children.insert(it, pNode);
		}
		endInsertRows();
	}
	else
	{
		beginInsertRows(parent, position, position + rows - 1);
		for (INT32 nIndex = 0; nIndex < rows; nIndex++)
		{
			BT_EDIT_NOD* pTree = m_pBTreeFile->NewTree(NULL, position + nIndex );
			LOG_PROCESS_ERROR(pTree);
		}
		endInsertRows();
    }

	m_pBTreeFile->SetModified();

	bResult = true;
Exit0:
    m_ResultByCommands = bResult;
    return bResult;
}

bool BTModel::cloneNodTreeByCommands(int position, const QModelIndex &parent, BT_EDIT_NOD* pRootNod )
{
    m_ResultByCommands = false;

    bool bResult = false;
    std::list<BT_EDIT_NOD*>::iterator it;

    //LOG_PROCESS_ERROR(parent.isValid() ); // shouldn't clone the top level tree
    if (parent.isValid())
    {
        BT_EDIT_NOD* pParent = (BT_EDIT_NOD*)parent.internalPointer();
        LOG_PROCESS_ERROR(pParent);

        it = pParent->Children.begin();
        INT32 nCounter = position;
        while (nCounter > 0)
        {
            LOG_PROCESS_ERROR(it != pParent->Children.end());
            nCounter--;
            ++it;
        }
    
        beginInsertRows(parent, position, position);

        BT_EDIT_NOD*  pNod = CloneNod(pRootNod);
        Q_ASSERT(pNod);
        pNod->pParent = pParent;
        pParent->Children.insert(it, pNod);

        endInsertRows();
    }
    else
    {
        beginInsertRows(parent, position, position);

        BT_EDIT_NOD*  pNod = CloneNod(pRootNod);
        Q_ASSERT(pNod);

        BT_EDIT_NOD* pTree = m_pBTreeFile->NewTree( pNod, position);
        LOG_PROCESS_ERROR(pTree);

        endInsertRows();
    }

    m_pBTreeFile->SetModified();

    //emitItemFocused(parent.child(position, 0));

    bResult = true;
Exit0:
    m_ResultByCommands = bResult;
    return bResult;
}

bool BTModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    return false;
}

bool BTModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    //cmd hold data first
    QModelIndex CurrentIndex = index(position, 0, parent);
    LOG_PROCESS_ERROR(CurrentIndex.isValid());

    BT_EDIT_NOD* pNod = (BT_EDIT_NOD*)CurrentIndex.internalPointer();
    LOG_PROCESS_ERROR( pNod );
    
    int nParentNodIndex = -1;
    if ( parent.isValid() )
    {
        nParentNodIndex = GetBTreeFile()->GetNodeIndex((BT_EDIT_NOD*)parent.internalPointer());
        LOG_PROCESS_ERROR( nParentNodIndex >= 0 );
    }

    RemoveCommand *pCmd = new RemoveCommand( pNod, nParentNodIndex, position, rows, this );
    Q_ASSERT(pCmd);
    m_pUndoStack->push(pCmd);  // execute redo, and set result to m_ResultByCommands

    emitModelDirty();
    if(position > 0)
    {
        QModelIndex focusIndex = index(position - 1, 0, parent); 
        emitItemFocused(focusIndex);
    }
Exit0:
    return m_ResultByCommands;
}

bool BTModel::removeRowsByCommands(int position, int rows, const QModelIndex &parent)
{
    m_ResultByCommands = false;

    //bool bResult = false;
    BOOL bResult = FALSE;
	std::list<BT_EDIT_NOD*>::iterator it;

	if (parent.isValid())
	{
		BT_EDIT_NOD* pParent = (BT_EDIT_NOD*)parent.internalPointer();
		LOG_PROCESS_ERROR(pParent);

		it = pParent->Children.begin();
		INT32 nCounter = position;
		while (nCounter > 0 && it != pParent->Children.end())
		{
			nCounter--;
			++it;
		}
		LOG_PROCESS_ERROR(it != pParent->Children.end());

		beginRemoveRows(parent, position, position + rows - 1);
		for (INT32 nIndex = 0; nIndex < rows; nIndex++)
        {
            BT_EDIT_NOD* pNode = *it;
            DestroyNod( pNode );

            //if (pNode->Children.size() > 0)
            //{
            //    QModelIndex currentIndex;
            //    //currentIndex = createIndex(position+nIndex, 0, pNode);
            //    currentIndex = index(position+nIndex, 0, parent);
            //    bResult = removeRowsByCommands(0, pNode->Children.size(), currentIndex);
            //    LOG_PROCESS_ERROR( bResult );
            //}
            //SAFE_DELETE(pNode);

			it = pParent->Children.erase(it);
		}
		endRemoveRows();
	}
	else
	{
        LOG_PROCESS_ERROR( position < GetBTreeFile()->GetTreeCount() );
        LOG_PROCESS_ERROR( position + rows - 1 < GetBTreeFile()->GetTreeCount() );

		beginRemoveRows(parent, position, position + rows - 1);
		for (INT32 nIndex = 0; nIndex < rows; nIndex++)
        {
            bResult = m_pBTreeFile->DelTree(position + nIndex);
            LOG_PROCESS_ERROR( bResult );
		}
		endRemoveRows();
	}

	m_pBTreeFile->SetModified();

	bResult = true;
Exit0:
    m_ResultByCommands = (bool)bResult;
	return bResult;
}

const QString ActionTag("ACTION");
const QString ActionName("NAME");
const QString Param0("PARAM0");
const QString Param1("PARAM1");
const QString Param2("PARAM2");
const QString Param3("PARAM3");

BT_EDIT_NOD * BTModel::restoreNod(QXmlStreamReader *reader)
{
    BT_EDIT_NOD *pNod = NULL;

    while (!reader->atEnd()) 
    {
        reader->readNext();
        if (reader->isStartElement()) 
        {
            if (reader->name() == ActionTag) 
            {
                const QString name = reader->attributes().value(ActionName).toString();
                const QString p0 = reader->attributes().value(Param0).toString();
                const QString p1 = reader->attributes().value(Param1).toString();
                const QString p2 = reader->attributes().value(Param2).toString();
                const QString p3 = reader->attributes().value(Param3).toString();

                BT_EDIT_NOD *new_nod = new BT_EDIT_NOD;
                Q_ASSERT(new_nod);
                strxcpy( new_nod->szType, name.toAscii(), MAX_BT_STRING_LEN);
                strxcpy( new_nod->szParam[0], p0.toAscii(), MAX_BT_STRING_LEN);
                strxcpy( new_nod->szParam[1], p1.toAscii(), MAX_BT_STRING_LEN);
                strxcpy( new_nod->szParam[2], p2.toAscii(), MAX_BT_STRING_LEN);
                strxcpy( new_nod->szParam[3], p3.toAscii(), MAX_BT_STRING_LEN);

                new_nod->pParent = pNod;
                if (pNod)
                {
                    pNod->Children.push_back(new_nod);
                }
                pNod = new_nod;
            }
        }
        else if (reader->isEndElement()) 
        {
            if (reader->name() == ActionTag) 
            {
                Q_ASSERT(pNod);
                if( NULL == pNod->pParent )
                    break;

                pNod = pNod->pParent;
                Q_ASSERT(pNod);
            }
        }
    }
    return pNod;
}

void BTModel::storeNod(QXmlStreamWriter *writer, BT_EDIT_NOD *nod) const
{
    writer->writeStartElement(ActionTag);
    writer->writeAttribute(ActionName, nod->szType);

    writer->writeAttribute(Param0,nod->szParam[0]);
    writer->writeAttribute(Param1,nod->szParam[1]);
    writer->writeAttribute(Param2,nod->szParam[2]);
    writer->writeAttribute(Param3,nod->szParam[3]);

    std::list<BT_EDIT_NOD*>::iterator it;
    for (it = nod->Children.begin(); it!=nod->Children.end(); ++it )
    {
        storeNod(writer, *it);
    }

    writer->writeEndElement();
}

bool BTModel::dropMimeData( const QMimeData * mimeData, Qt::DropAction action, int row, int column, const QModelIndex & parent )
{
    if ( !parent.isValid() )
        return false;
    if (action == Qt::IgnoreAction)
        return true;
    if (action != Qt::MoveAction || column > 0 || !mimeData || !mimeData->hasFormat(BTMimeType))
        return false;

    QByteArray xmlData = qUncompress(mimeData->data(BTMimeType));
    QXmlStreamReader reader(xmlData);
    BT_EDIT_NOD* pNod = restoreNod(&reader);
    Q_ASSERT( pNod );

    BT_EDIT_NOD *pParentNod = (BT_EDIT_NOD *)parent.internalPointer();
    Q_ASSERT( pParentNod );
    int nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNod);
    Q_ASSERT( nParentNodIndex >= 0 );

    InsertCommand *pCmd = new InsertCommand( pNod, nParentNodIndex, 0, 1, this );
    Q_ASSERT(pCmd);
    m_pUndoStack->push(pCmd);    // execute redo, and set result to m_ResultByCommands

    DestroyNod(pNod);
    
    emitModelDirty();
    return m_ResultByCommands;
}

QMimeData *BTModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.count() > 0);
    QModelIndex CurrentIndex = indexes.at(0);
    if (! CurrentIndex.parent().isValid() )    // top level shouldn't be dragged
    {
        return NULL;
    }

    if (BT_EDIT_NOD *item = Index2Nod(indexes.at(0))) 
    {
        QMimeData *mimeData = new QMimeData;
        QByteArray xmlData;
        QXmlStreamWriter writer(&xmlData);
        storeNod(&writer, item);
        mimeData->setData(BTMimeType, qCompress(xmlData, MaxCompression));
        return mimeData;
    }
    return NULL;
}

BT_EDIT_NOD* BTModel::Index2Nod(const QModelIndex &index) const
{
    return static_cast<BT_EDIT_NOD*>(index.internalPointer());
}

QModelIndex  BTModel::Nod2Index(const BT_EDIT_NOD *nod, int col) const
{
    return Nod2Index( QModelIndex(), nod, col );
}

QModelIndex  BTModel::Nod2Index(const QModelIndex& parent, const BT_EDIT_NOD *nod, int col) const
{
    if (NULL == nod)
        return QModelIndex();
    if (col >= columnCount(parent) )
        return QModelIndex();

    for (int row = 0; row < rowCount(parent); ++row) 
    {
        QModelIndex thisIndex = index(row, col, parent);
        if (thisIndex.isValid())
        {
            BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>( thisIndex.internalPointer() ) ;
            Q_ASSERT( pNode );
            if( pNode == nod )
            {
                return thisIndex;
            }
            else
            {
                QModelIndex sonIndex = Nod2Index(thisIndex, nod, col);
                if ( sonIndex.isValid() )
                {
                    return sonIndex;
                }
            }
        }
        else
        {
            return QModelIndex();
        }
    }
    return QModelIndex();
}

BT_EDIT_NOD* BTModel::ChildNod(const QModelIndex& ParentIndex, int nPos)
{
    int nChildCount = 0;
    BT_EDIT_NOD* pResult = NULL;

    if ( ParentIndex.isValid() )
    {
        BT_EDIT_NOD *ParentNod = Index2Nod(ParentIndex);
        Q_ASSERT(ParentNod);
        nChildCount = rowCount(ParentIndex);
        Q_ASSERT(0 <= nPos && nPos < nChildCount);

        std::list<BT_EDIT_NOD*>::iterator it = ParentNod->Children.begin();
        for ( int i=0; i < nChildCount; ++i )
        {
            Q_ASSERT( it!=ParentNod->Children.end() );
            if( i == nPos )
            {
                pResult = *it;
                break;
            }
            ++it;
        }
    }
    else
    {
        nChildCount = GetBTreeFile()->GetTreeCount();
        Q_ASSERT(0 <= nPos && nPos < nChildCount);

        pResult = GetBTreeFile()->GetRootNode( nPos );
    }
Exit0:
    return pResult;
}

bool BTModel::moveToByCommands(QModelIndex& SrcParentIndex, int nSrcPos, QModelIndex& DstParentIndex, int nDstPos )
{
    m_ResultByCommands = false;

    Q_ASSERT( !((SrcParentIndex == DstParentIndex) && (nSrcPos == nDstPos) ) );

    BT_EDIT_NOD* SrcNod = ChildNod(SrcParentIndex, nSrcPos);

    BT_EDIT_NOD* pSrcTmp = CloneNod(SrcNod);

    removeRowsByCommands(nSrcPos, 1, SrcParentIndex);
    cloneNodTreeByCommands(nDstPos, DstParentIndex, pSrcTmp );

    DestroyNod(pSrcTmp);

    QModelIndex focusIndex = index(nDstPos, 0, DstParentIndex );
    emitItemFocused(focusIndex);
    emitModelDirty();

    m_ResultByCommands = true;
    return m_ResultByCommands;
}

bool BTModel::swapItemByCommands(QModelIndex& SrcParentIndex, int nSrcPos, QModelIndex& DstParentIndex, int nDstPos )
{
    m_ResultByCommands = false;

    Q_ASSERT( !((SrcParentIndex == DstParentIndex) && (nSrcPos == nDstPos) ) );

    BT_EDIT_NOD* SrcNod = ChildNod(SrcParentIndex, nSrcPos);
    BT_EDIT_NOD* DstNod = ChildNod(DstParentIndex, nDstPos);

    BT_EDIT_NOD* pSrcTmp = CloneNod(SrcNod);
    BT_EDIT_NOD* pDstTmp = CloneNod(DstNod);

    removeRowsByCommands(nSrcPos, 1, SrcParentIndex);
    cloneNodTreeByCommands(nSrcPos, SrcParentIndex, pDstTmp );
    removeRowsByCommands(nDstPos, 1, DstParentIndex);
    cloneNodTreeByCommands(nDstPos, DstParentIndex, pSrcTmp );

    DestroyNod(pSrcTmp);
    DestroyNod(pDstTmp);

    QModelIndex focusIndex = index(nDstPos, 0, DstParentIndex );
    emitItemFocused(focusIndex);
    emitModelDirty();

    m_ResultByCommands = true;
    return m_ResultByCommands;
}

void BTModel::emitItemFocused( const QModelIndex &index)
{
    emit ItemFocused( index );
}

void BTModel::emitModelDirty(void)
{
    emit ModelDirty( GetBTreeFile()->IsModified() );

    //if( !m_pBTreeFile->IsModified() )
    //    return ModelDirty( false );
    //emit ModelDirty( m_pUndoStack->canUndo() ); // m_pBTreeFile->IsModified() );//
}

bool BTModel::moveUp(const QModelIndex &index )
{
    if (!index.isValid())
        return false;

    MoveToCommand *pCmd = new MoveToCommand( index, nmtdPrior, this );
    Q_ASSERT(pCmd);
    if( !pCmd->MakeSureDst() )
        return false;

    m_pUndoStack->push(pCmd);   // execute redo, and set result to m_ResultByCommands

    return m_ResultByCommands;
}

bool BTModel::moveDown(const QModelIndex &index )
{
    if (!index.isValid())
        return false;

    BT_EDIT_NOD *pNod = Index2Nod(index);
    Q_ASSERT(pNod);

    MoveToCommand *pCmd = new MoveToCommand( index, nmtdNext, this );
    Q_ASSERT(pCmd);
    if( !pCmd->MakeSureDst() )
        return false;
    m_pUndoStack->push(pCmd);   // execute redo, and set result to m_ResultByCommands

    return m_ResultByCommands;
}

bool BTModel::promote(const QModelIndex &index )
{
    if (!index.isValid())
        return false;

    MoveToCommand *pCmd = new MoveToCommand( index, nmtdPromote, this );
    Q_ASSERT(pCmd);
    if( !pCmd->MakeSureDst() )
        return false;
    m_pUndoStack->push(pCmd);   // execute redo, and set result to m_ResultByCommands

    return m_ResultByCommands;
}

bool BTModel::demote(const QModelIndex &index )
{
    if (!index.isValid())
        return false;

    MoveToCommand *pCmd = new MoveToCommand( index, nmtdDemote, this );
    Q_ASSERT(pCmd);
    if( !pCmd->MakeSureDst() )
        return false;
    m_pUndoStack->push(pCmd);   // execute redo, and set result to m_ResultByCommands

    return m_ResultByCommands;
}

bool BTModel::CopyTree(const QModelIndex& index )
{
    BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
    Q_ASSERT(pNode);

    if ( m_pHoldTree )
    {
        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;
    }
    m_pHoldTree = CloneNod(pNode);
    m_pHoldTree->pParent = NULL;

    return true;
}
bool BTModel::CopyNode(const QModelIndex& index )
{
    BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
    Q_ASSERT(pNode);

    if ( m_pHoldTree )
    {
        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;
    }
    m_pHoldTree = new BT_EDIT_NOD;
    Q_ASSERT(m_pHoldTree);
    memcpy( m_pHoldTree->szType, pNode->szType, sizeof(m_pHoldTree->szType) );
    memcpy( m_pHoldTree->szParam, pNode->szParam, sizeof(m_pHoldTree->szParam) );
    m_pHoldTree->pParent = NULL;

    return true;
}

bool BTModel::CutTree(const QModelIndex& index )
{
    BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
    Q_ASSERT(pNode);

    if ( m_pHoldTree )
    {
        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;
    }
    m_pHoldTree = CloneNod(pNode);
    m_pHoldTree->pParent = NULL;

    return removeRows(index.row(), 1, index.parent() );
}

bool BTModel::CutNode(const QModelIndex& index )
{
    BT_EDIT_NOD* pNode = static_cast<BT_EDIT_NOD*>(index.internalPointer());
    Q_ASSERT(pNode);

    if ( m_pHoldTree )
    {
        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;
    }
    m_pHoldTree = new BT_EDIT_NOD;
    Q_ASSERT(m_pHoldTree);
    memcpy( m_pHoldTree->szType, pNode->szType, sizeof(m_pHoldTree->szType) );
    memcpy( m_pHoldTree->szParam, pNode->szParam, sizeof(m_pHoldTree->szParam) );
    m_pHoldTree->pParent = NULL;

    return removeRows(index.row(), 1, index.parent() );

    //BT_EDIT_NOD* pParentNode = static_cast<BT_EDIT_NOD*>( index.parent().internalPointer() );
    //int nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNode);
    //RemoveCommand *pCmd = new RemoveCommand( m_pHoldTree, nParentNodIndex, index.row(), 1, this );
    //Q_ASSERT(pCmd);
    //m_pUndoStack->push(pCmd);    // execute redo, and set result to m_ResultByCommands

    //if ( m_ResultByCommands )
    //{
    //    emitModelDirty();
    //}

    //return m_ResultByCommands;
}

bool BTModel::PasteAsChild( const QModelIndex& parent )
{
    if ( m_pHoldTree )
    {
        BT_EDIT_NOD *pParentNod = (BT_EDIT_NOD *)parent.internalPointer();
        Q_ASSERT( pParentNod );
        int nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNod);
        Q_ASSERT( nParentNodIndex >= 0 );

        InsertCommand *pCmd = new InsertCommand( m_pHoldTree, nParentNodIndex, 0, 1, this );
        Q_ASSERT(pCmd);
        m_pUndoStack->push(pCmd);    // execute redo, and set result to m_ResultByCommands

        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;

        QModelIndex focusIndex = index(0, 0, parent );
        emitItemFocused(focusIndex);
        emitModelDirty();

        return m_ResultByCommands;
    }

    return false;
}

bool BTModel::PasteAsSibling( const QModelIndex& current )
{
    if ( m_pHoldTree )
    {
        int nParentNodIndex = -1;
        BT_EDIT_NOD *pParentNod = (BT_EDIT_NOD *)current.parent().internalPointer();
        if( pParentNod )
        {
            nParentNodIndex = GetBTreeFile()->GetNodeIndex(pParentNod);
            Q_ASSERT( nParentNodIndex >= 0 );
        }

        InsertCommand *pCmd = new InsertCommand( m_pHoldTree, nParentNodIndex, current.row()+1, 1, this );
        Q_ASSERT(pCmd);
        m_pUndoStack->push(pCmd);    // execute redo, and set result to m_ResultByCommands

        DestroyNod(m_pHoldTree);
        m_pHoldTree = NULL;

        QModelIndex focusIndex = index(current.row()+1, 0, current );
        emitItemFocused(focusIndex);
        emitModelDirty();

        return m_ResultByCommands;
    }

    return false;
}

bool BTModel::HasHoldTree(void)
{
    return m_pHoldTree ? true : false;
}


BOOL CALLBACK WindowTraverseFunc( HWND hwnd, LPARAM lParam )
{
    TCHAR* pszTargetStr = TEXT("Unreal Editor for");
    TCHAR szCaptain[128];
    int nRetCode = GetWindowText( hwnd, szCaptain, 128 );
    if(lstrlen(szCaptain) > 0)
    {
        TCHAR* pChar = strstr( szCaptain, pszTargetStr );
        if ( pChar )
        {
            *((HWND*)lParam) = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

bool BTModel::ParseSUIDInfo( const QModelIndex& current, char** pcszType, int& nID )
{
    QString strText = data(current, Qt::DisplayRole).toString();

    if( m_pBTreeFile->IsTreeRoot( (BT_EDIT_NOD*)current.internalPointer() ) )
    {
        bool ret = m_pBTreeFile->ParseEvent( strText.toAscii(), pcszType, nID );
        if (ret)
        {
            return true;
        }
    }
    else if( current.column() > 0 )
    {
        nID = atoi( strText.toAscii() );
        if ( nID <= 0 )
        {
            return false;
        }

        QModelIndex& ActionIndex = index(current.row(), 0, current.parent() );
        if ( !ActionIndex.isValid() )
        {
            return false;
        }

        QString strActionText = data(ActionIndex, Qt::DisplayRole).toString();

        bool ret = m_pBTreeFile->ParseAction( strActionText.toAscii(), current.column() - 1, pcszType );
        if (ret)
        {
            return true;
        }
    }

    return false;
}

bool BTModel::UnrealLocation( const QModelIndex& current )
{
    int nID = -1;
    char* pcszType = NULL;
    bool ret = ParseSUIDInfo( current, &pcszType, nID );
    if ( !ret )
    {
        return false;
    }
    //sprintf( szType, "NPCNewSpawnGroupMediumActor" );	// type
    //nID = 2;

    COPYDATASTRUCT oData = {};
    oData.lpData = pcszType;
    oData.cbData = strlen(pcszType) + 1;
    oData.dwData = nID;									// ID

    HWND hWnd = NULL;				//
    //TCHAR* pszCaptain = TEXT("MsgToUE3");//_TEXT("calculator");
    //hWnd = FindWindowEx(NULL, 0, NULL, pszCaptain );
    int nRetCode = EnumWindows( (WNDENUMPROC)WindowTraverseFunc, (LPARAM)(&hWnd) );
    if ( NULL == hWnd )
    {
        return false;
    }

    SendMessage( hWnd, WM_COPYDATA, (WPARAM)7777, (LPARAM)&oData );

    return true;
}