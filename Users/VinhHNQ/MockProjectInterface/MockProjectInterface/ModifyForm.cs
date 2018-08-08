using OfficeOpenXml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MockProjectInterface
{
    public partial class ModifyForm : Form
    {
        private List<CWheelChair> list = new List<CWheelChair>();

        public ModifyForm()
        {
            InitializeComponent();
            LoadData();
            cbID.DataSource = getIDArray();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            CWheelChair oldValue = getCWheelbyID(cbID.Text);
            CWheelChair newValue = new CWheelChair() {
                id = cbID.Text,
                userName = txbUser.Text,
                roomID=txbRoom.Text,
                blockID=txbBlock.Text
            };
            int index = list.IndexOf(oldValue);
            if (index != -1)
                list[index] = newValue;
            SaveData();
            Close();
        }

        private List<string> getIDArray()
        {
            List<string> arr=new List<string>();
            foreach (CWheelChair item in list)
            {
                arr.Add(item.id);
            }
            return arr;
        }


        //Save data to excel file
        private void SaveData()
        {
            var package = new ExcelPackage(new FileInfo(@"..\Resources\ImportData.xlsx"));

            ExcelWorksheet worksheet = package.Workbook.Worksheets[1];
            for(int i= worksheet.Dimension.Start.Row + 1; i <= worksheet.Dimension.End.Row; i++)
            {
                int j = 1;

                if (worksheet.Cells[i, j].Value == null)
                {
                    continue;
                }
                worksheet.Cells[i, j++].Value = null;
                worksheet.Cells[i, j++].Value = null;
                worksheet.Cells[i, j++].Value = null;
                worksheet.Cells[i, j++].Value = null;
                worksheet.Cells[i, j++].Value = null;
            }

            for (int i = 0; i < list.Count ; i++)
            {
                int j = 1;
                worksheet.Cells[i + 2, j++].Value = list[i].id;
                worksheet.Cells[i + 2, j++].Value = list[i].userName;
                worksheet.Cells[i + 2, j++].Value = list[i].roomID;
                worksheet.Cells[i + 2, j++].Value = list[i].blockID;
                worksheet.Cells[i + 2, j++].Value = 1;
            }
            package.Save();
        }

        //Load data from excel
        private void LoadData()
        {
            var package = new ExcelPackage(new FileInfo(@"..\Resources\ImportData.xlsx"));

            ExcelWorksheet worksheet = package.Workbook.Worksheets[1];

            for (int i = worksheet.Dimension.Start.Row + 1; i <= worksheet.Dimension.End.Row; i++)
            {
                int j = 1;

                if (worksheet.Cells[i, j].Value == null)
                {
                    continue;
                }
                string name = worksheet.Cells[i, j++].Value.ToString();
                string userName = worksheet.Cells[i, j++].Value.ToString();
                string roomID = worksheet.Cells[i, j++].Value.ToString();
                string blockID = worksheet.Cells[i, j++].Value.ToString();
                string status = worksheet.Cells[i, j++].Value.ToString();


                CWheelChair cWheel = new CWheelChair()
                {
                    id = name,
                    userName = userName,
                    roomID = roomID,
                    blockID = blockID,
                    status = status
                };
                list.Add(cWheel);
            }
        }

        private CWheelChair getCWheelbyID(string id)
        {
            foreach (CWheelChair item in list)
            {
                if (item.id == id) { return item; }
            }
            return null;
        }

        private void cbID_TextChanged(object sender, EventArgs e)
        {
            CWheelChair cWheelChair = getCWheelbyID(cbID.Text);
            txbUser.Text = cWheelChair.userName;
            txbRoom.Text = cWheelChair.roomID;
            txbBlock.Text = cWheelChair.blockID;
        }
    }
}
