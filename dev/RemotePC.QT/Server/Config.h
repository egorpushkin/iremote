/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CONFIG_H__REMOTEPC__INCLUDED_
#define CONFIG_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	class Config
	{
	protected:

		/** Allows to protect class constructor. */
		friend struct Loki::CreateUsingNew< Config >;

		Config();

	public:

		static Config& Instance();

		void Load(const std::string& path);

		void Save() const;

	public:

		std::string GetPolicy() const;

		std::string GetService() const;
		void SetService(const std::string& service);

		bool ShareScreenshots() const;
		void SetScreenshotsSharing(bool share);

		float GetFPS() const;
		void SetFPS(float fps);

		int GetVolumeUpdateInterval() const;

		mc::Guid GetHostId() const;

        bool IsSFBEnabled() const;
        void SetSFBEnabled(bool enabled);

        float GetSFBFPS() const;
        void SetSFBFPS(float fps);

        typedef enum tagSFBFormat_
        {
            SFB_TRUECOLOR = 0,
            SFB_GRAYSCALE = 1
        }
        SFBFormat_;

        SFBFormat_ GetSFBPixelFormat() const;
        void SetSFBPixelFormat(SFBFormat_ format);

        int GetSFBCompression() const;
        void SetSFBCompression(int compression);

        size_t GetSFBCellSize() const;
        void SetSFBCellSize(size_t size);

        void SetSFBChanged(bool changed);
        bool IsSFBChanged() const;
        void FlushFlags();

	private:

		void SetPolicy(const std::string& policy);

		void SetHostId(mc::RefClsid hostId);

		std::string GetPath() const;

	private:

		friend class ConfigParser;

		std::string path_;

		std::string policy_;

		std::string service_;

		bool screenshotSharing_;

		float fps_;

		int volumeUpdateInterval_;

		bool hasHostId_;

		mc::Guid hostId_;

        bool sfbEnabled_;

        float sfbFps_;

        SFBFormat_ sfbPixelFormat_;

        int sfbCompression_;

        size_t sfbCellSize_;

        bool sfbChanged_;

	};

}

#endif // !CONFIG_H__REMOTEPC__INCLUDED_
