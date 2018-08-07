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
    public partial class Add : Form
    {
        private List<CWheelChair> list = new List<CWheelChair>();
        public Add()
        {
            InitializeComponent();
            LoadData();
            txbID.Text = GenerateID();
            txbID.GotFocus += textBox_GotFocus;
        }

        private string GenerateID()
        {
            List<string> listLastChar = getListLastChar();
            if (list.Count < Int16.Parse(listLastChar[listLastChar.Count - 1]))
            {
                for(int i=1;i<=listLastChar.Count;i++)
                {
                    if (listLastChar.Contains(i.ToString())==false)
                    {
                        return "WC" + i.ToString();
                    }
                }
            }
            return "WC" + (list.Count + 1).ToString();
        }

        private List<string> getListLastChar()
        {
            List<string> listLastChar =new List<string>();
            List<int> tmp = new List<int>();
            foreach(CWheelChair item in list)
            {
                tmp.Add(Int16.Parse(item.id[2].ToString()));
            }
            SelectionSort(tmp);
            foreach(int item in tmp)
            {
                listLastChar.Add(item.ToString());
            }
            return listLastChar;
        }

        private void SelectionSort(List<int> data)
        {
            int min;
            for (int i = 0; i < data.Count - 1; i++)
            {
                min = i;

                for (int index = i + 1; index < data.Count; index++)
                {
                    if (data[index] < data[min])
                    {
                        min = index;
                    }
                }
                swap(i, min,data);
            }
        }

        public void swap(int thu_nhat, int thu_hai,List<int> data)
        {
            int bien_tam = data[thu_nhat];
            data[thu_nhat] = data[thu_hai];
            data[thu_hai] = bien_tam;
        }

        private void textBox_GotFocus(object sender, EventArgs e)
        {
            ((TextBox)sender).Parent.Focus();

        }

        private void button1_Click(object sender, EventArgs e)
        {
            SaveData();
        }

        //Save data to excel file
        private void SaveData()
        {
            var package = new ExcelPackage(new FileInfo(@"..\..\ImportData.xlsx"));

            ExcelWorksheet worksheet = package.Workbook.Worksheets[1];
            int j = 1;
            int i = worksheet.Dimension.End.Row + 1;
            worksheet.Cells[i, j++].Value = txbID.Text;
            worksheet.Cells[i, j++].Value = txbUser.Text;
            worksheet.Cells[i, j++].Value = txbRoom.Text;
            worksheet.Cells[i, j++].Value = txbBlock.Text;
            worksheet.Cells[i, j++].Value = 1;

            package.Save();
            Close();
        }

        //Load data from excel
        private void LoadData()
        {
            var package = new ExcelPackage(new FileInfo(@"..\..\ImportData.xlsx"));

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
    }
}
