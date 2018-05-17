defmodule NifXsdValidate.MixProject do
  use Mix.Project

  @version "0.0.2"

  def project do
    [
      app: :nif_xsd_validate,
      version: @version,
      elixir: "~> 1.4",
      package: [
        maintainers: [],
        licenses: ["terminalstatic"],
        links: %{"Github" => "https://github.com/terminalstatic/nif_xsd_validate"},
        files: [
          "mix.exs",
          "Makefile",
          "README.md",
          "config",
          "lib",
          "priv/.gitkeep",
          "c_source"
        ]
      ],
      compilers: [:validateXsd] ++ Mix.compilers,
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end
defmodule Mix.Tasks.Compile.ValidateXsd do
  def run(_args) do
    {result, _errcode} = System.cmd("make", ["priv/validateXsd.so"], stderr_to_stdout: true)
    IO.binwrite(result)
  end
end
