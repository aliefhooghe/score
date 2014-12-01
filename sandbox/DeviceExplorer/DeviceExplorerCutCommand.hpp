#pragma once

#include "command/SerializableCommand.hpp"

#include "DeviceExplorerModel.hpp"


class DeviceExplorerCutCommand : public iscore::SerializableCommand
{
public:

  DeviceExplorerCutCommand();
  
  void set(const QModelIndex &parentIndex, int row, 
	   const QString &text,
	   DeviceExplorerModel *model);


  virtual void undo() override;
  virtual void redo() override;
  virtual int id() const override;
  virtual bool mergeWith(const QUndoCommand* other) override;
  
protected:
  virtual void serializeImpl(QDataStream&) override;
  virtual void deserializeImpl(QDataStream&) override;

  

protected:
  DeviceExplorerModel *m_model;
  QByteArray m_data;
  DeviceExplorerModel::Path m_parentPath;
  int m_row;
};
