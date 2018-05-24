defmodule NifXsdValidate.MixProject do
  use Mix.Project

  @version "0.0.10"

  def project do
    [
      app: :nif_xsd_validate,
      version: @version,
      elixir: "~> 1.6",
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
      compilers: [:nifXsdValidate] ++ Mix.compilers(),
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      # ExDoc
      name: "NifXsdValidate",
      source_url: "https://github.com/terminalstatic/nif_xsd_validate"
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      applications: []
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ex_doc, "~> 0.18.3", only: :dev, runtime: false},
      {:dialyxir, "~> 0.5", only: [:dev], runtime: false}
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end

defmodule Mix.Tasks.Compile.NifXsdValidate do
  def run(_args) do
    if match?({:win32, _}, :os.type()) do
      IO.warn("Windows is not supported.")
      exit(1)
    else
      {_, errcode} = System.cmd("make", [], into: IO.stream(:stdio, :line))

      if errcode != 0 do
        Mix.raise("Mix task failed")
      end

      if Mix.env() == :prod do
        IO.puts("Applying :prod build hack")
        basename = Path.basename(Mix.Project.app_path())

        ('cp -rf ' ++
           String.to_charlist(
             Path.join(Mix.Project.deps_path(), [basename, "/", "priv"]) <>
               " " <> Mix.Project.app_path()
           ))
        |> :os.cmd()
      end
    end

    :ok
  end
end
